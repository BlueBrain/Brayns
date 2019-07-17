/* Copyright (c) 2019, EPFL/Blue Brain Project
 *
 * This file is part of Brayns <https://github.com/BlueBrain/Brayns>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "streamer.h"

#include <cstdio>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>

#include <brayns/engine/Camera.h>
#include <brayns/engine/Engine.h>
#include <brayns/engine/FrameBuffer.h>
#include <brayns/engine/Material.h>
#include <brayns/engine/Model.h>
#include <brayns/engine/Renderer.h>
#include <brayns/engine/Scene.h>
#include <brayns/parameters/ParametersManager.h>
#include <brayns/pluginapi/PluginAPI.h>

#include <brayns/common/light/Light.h>

#include <ospray/mpiCommon/MPICommon.h>
#include <ospray/ospcommon/networking/BufferedDataStreaming.h>

// mpv /tmp/test.sdp --no-cache --untimed --vd-lavc-threads=1 -vf=flip
// mpv /tmp/test.sdp --profile=low-latency --vf=vflip

namespace ospcommon
{
namespace networking
{
template <typename T>
inline WriteStream &operator<<(WriteStream &buf, const std::array<T, 3> &rh)
{
    buf.write((const byte_t *)rh.data(), sizeof(T) * 3);
    return buf;
}

template <typename T>
inline ReadStream &operator>>(ReadStream &buf, std::array<T, 3> &rh)
{
    buf.read((byte_t *)rh.data(), sizeof(T) * 3);
    return buf;
}

inline WriteStream &operator<<(WriteStream &buf, const brayns::PropertyMap &rh)
{
    using namespace brayns;
    buf << rh.getProperties().size();
    for (auto prop : rh.getProperties())
    {
        buf << prop->type << prop->name;
        switch (prop->type)
        {
        case brayns::Property::Type::Double:
            buf << prop->get<double>();
            break;
        case Property::Type::Int:
            buf << prop->get<int32_t>();
            break;
        case Property::Type::String:
            buf << prop->get<std::string>();
            break;
        case Property::Type::Bool:
            buf << prop->get<bool>();
            break;
        case Property::Type::Vec2d:
            buf << prop->get<std::array<double, 2>>();
            break;
        case Property::Type::Vec2i:
            buf << prop->get<std::array<int32_t, 2>>();
            break;
        case Property::Type::Vec3d:
            buf << prop->get<std::array<double, 3>>();
            break;
        case Property::Type::Vec3i:
            buf << prop->get<std::array<int32_t, 3>>();
            break;
        case Property::Type::Vec4d:
            buf << prop->get<std::array<double, 4>>();
            break;
        }
    }
    return buf;
}

inline ReadStream &operator>>(ReadStream &buf, brayns::PropertyMap &rh)
{
    using namespace brayns;
    size_t sz;
    buf >> sz;
    for (size_t i = 0; i < sz; ++i)
    {
        Property::Type type;
        std::string name;
        buf >> type >> name;
        switch (type)
        {
        case brayns::Property::Type::Double:
        {
            double val;
            buf >> val;
            rh.setProperty({name, val});
            break;
        }
        case Property::Type::Int:
        {
            int32_t val;
            buf >> val;
            rh.setProperty({name, val});
            break;
        }
        case Property::Type::String:
        {
            std::string val;
            buf >> val;
            rh.setProperty({name, val});
            break;
        }
        case Property::Type::Bool:
        {
            bool val;
            buf >> val;
            rh.setProperty({name, val});
            break;
        }
        case Property::Type::Vec2d:
        {
            std::array<double, 2> val;
            buf >> val;
            rh.setProperty({name, val});
            break;
        }
        case Property::Type::Vec2i:
        {
            std::array<int32_t, 2> val;
            buf >> val;
            rh.setProperty({name, val});
            break;
        }
        case Property::Type::Vec3d:
        {
            std::array<double, 3> val;
            buf >> val;
            rh.setProperty({name, val});
            break;
        }
        case Property::Type::Vec3i:
        {
            std::array<int32_t, 3> val;
            buf >> val;
            rh.setProperty({name, val});
            break;
        }
        case Property::Type::Vec4d:
        {
            std::array<double, 4> val;
            buf >> val;
            rh.setProperty({name, val});
            break;
        }
        }
    }

    return buf;
}
}
}

#define THROW(msg)                         \
    {                                      \
        std::stringstream s;               \
        s << msg;                          \
        throw std::runtime_error(s.str()); \
    }

#define STREAM_PIX_FMT AV_PIX_FMT_YUV420P

#define USE_CUDA_HACK

namespace streamer
{
Streamer::Streamer(const brayns::PropertyMap &props)
    : _props(props)
{
}

Streamer::~Streamer()
{
    if (asyncEncode() && _encodeThread.joinable())
    {
        _images.push(Image{});
        _encodeThread.join();
    }

    if (pkt)
        av_packet_free(&pkt);

    if (streamContext)
    {
        if (streamContext->pb)
            avio_close(streamContext->pb);
        avformat_free_context(streamContext);
    }

    if (encoder)
        NvPipe_Destroy(encoder);
    avformat_network_deinit();
}

void Streamer::init()
{
    av_register_all();
    if (avformat_network_init() < 0)
        THROW("Could not init stream network");

    const bool useHEVC = _props.getProperty<bool>("hevc");
    encoder =
        NvPipe_CreateEncoder(NVPIPE_BGRA32, useHEVC ? NVPIPE_HEVC : NVPIPE_H264,
                             NVPIPE_LOSSY, bitrate(), fps());
    if (!encoder)
        THROW("Failed to create encoder: " << NvPipe_GetError(nullptr));

    const bool useRTP = !_props.getProperty<bool>("rtsp");

    const std::string filename =
        useRTP
            ? "rtp://" + _props.getProperty<std::string>("host")
            : "rtsp://" + _props.getProperty<std::string>("host") + "/test.sdp";

    AVOutputFormat *fmt =
        av_guess_format(useRTP ? "rtp" : "rtsp", nullptr, nullptr);
    avformat_alloc_output_context2(&streamContext, fmt,
                                   useHEVC ? "hevc" : "h264", filename.c_str());
    if (!streamContext)
        THROW("Could not open format context");

    if (!(streamContext->oformat->flags & AVFMT_NOFILE))
    {
        if (avio_open2(&streamContext->pb, streamContext->filename,
                       AVIO_FLAG_WRITE, nullptr, nullptr) < 0)
            THROW("Failed to open stream output context, stream will not work");
    }

    AVCodecID codecID = useHEVC ? AV_CODEC_ID_HEVC : AV_CODEC_ID_H264;
    codec = avcodec_find_encoder(codecID);
    if (!codec)
        THROW("Could not find encoder for "
              << std::quoted(avcodec_get_name(codecID)));

    stream = avformat_new_stream(streamContext, codec);
    if (!stream)
        THROW("Could not allocate stream");

    const AVRational avFPS = {fps(), 1};
    stream->time_base = av_inv_q(avFPS);

    stream->codecpar->codec_id = codecID;
    stream->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
    stream->codecpar->width = width();
    stream->codecpar->height = height();

    if (avformat_write_header(streamContext, nullptr) !=
        AVSTREAM_INIT_IN_WRITE_HEADER)
    {
        THROW("Could not write stream header")
    }

    _initMPI();
    if (useRTP)
    {
        char buf[200000];
        AVFormatContext *ac[] = {streamContext};
        av_sdp_create(ac, 1, buf, 20000);

        std::stringstream outFile;
        outFile << "/tmp/test";
        if (useMPI())
            outFile << mpicommon::world.rank;
        outFile << ".sdp";
        FILE *fsdp = fopen(outFile.str().c_str(), "w");
        if (fsdp)
        {
            fprintf(fsdp, "%s", buf);
            fclose(fsdp);
        }
    }

    pkt = av_packet_alloc();

    if (asyncEncode())
    {
        _encodeThread =
            std::thread(std::bind(&Streamer::_runAsyncEncode, this));
    }

#ifndef USE_CUDA_HACK
    _createFrameBuffers();
#endif

    auto model = _api->getScene().createModel();
    auto material = model->createMaterial(0, "white");
    material->setDiffuseColor({1.f, 1.f, 1.f});
    material->setSpecularColor({1.f, 1.f, 1.f});
    model->addSphere(0, {{0.f, 0.f, 0.f}, .1f});
    _lightModel = std::make_shared<brayns::ModelDescriptor>(std::move(model),
                                                            "LightScene");
    _api->getScene().addModel(_lightModel);

    _timer.start();
}

void Streamer::preRender()
{
    _swapFrameBuffer();
    _syncFrame();
}

void Streamer::postRender()
{
    auto frameBuffer = _getFrameBuffer();
    if (!frameBuffer)
        return;

#ifdef USE_CUDA_HACK
    if (asyncEncode() && useCudaBuffer())
        _images.waitSize(0);
    frameBuffer->map();
    const void *buffer = useCudaBuffer() ? frameBuffer->cudaBuffer()
                                         : frameBuffer->getColorBuffer();
#else
    frameBuffer->map();
    const void *buffer = frameBuffer->getColorBuffer();
#endif
    if (asyncEncode())
    {
        Image img(_frameNumber, frameBuffer->getSize());
        img.buffer = buffer;
        _images.push(img);
    }
    else
        encodeFrame(_frameNumber, frameBuffer->getSize(), buffer);

    frameBuffer->unmap();

    if (_props.getProperty<bool>("stats"))
        printStats();

    _nextFrame();
}

void Streamer::encodeFrame(const size_t frameNumber,
                           const brayns::Vector2ui &size, const void *data)
{
    brayns::Timer encodeTimer;
    encodeTimer.start();

    const auto pts =
        av_rescale_q(frameNumber, AVRational{1, fps()}, stream->time_base);

    static std::vector<uint8_t> compressed;
    const size_t bufSize = size.x * size.y * 4;
    if (compressed.size() < bufSize)
        compressed.resize(bufSize);

    const auto compressedSize =
        NvPipe_Encode(encoder, data, size.x * 4, compressed.data(),
                      compressed.size(), size.x, size.y,
                      gop() > 0 ? frameNumber % gop() == 0 : false);
    av_init_packet(pkt);
    pkt->data = compressed.data();
    pkt->size = compressedSize;
    pkt->pts = pts;
    pkt->dts = pts;
    pkt->stream_index = stream->index;

    if (!memcmp(compressed.data(), "\x00\x00\x00\x01\x67", 5))
        pkt->flags |= AV_PKT_FLAG_KEY;

    encodeDuration = encodeTimer.elapsed();

    streamFrame();
}

void Streamer::streamFrame()
{
    _barrier();
    av_interleaved_write_frame(streamContext, pkt);
    av_packet_unref(pkt);
}

bool Streamer::useCudaBuffer() const
{
    return _api->getParametersManager()
               .getApplicationParameters()
               .getEngine() == "optix";
}

bool Streamer::isLocalOrMaster() const
{
    return !useMPI() || mpicommon::IamTheMaster();
}

void Streamer::_runAsyncEncode()
{
    while (_api->getEngine().getKeepRunning())
    {
        Image img = _images.pop();
        const void *data = img.buffer;
        if (!img.data.empty())
            data = reinterpret_cast<const uint8_t *const>(img.data.data());
        if (!data)
            break;
        encodeFrame(img.frameNumber, img.size, data);
    }
}

void Streamer::printStats()
{
    bool flushOnly = true;

    if (useMPI())
    {
        if (_props.getProperty<bool>("master-stats"))
        {
            if (!mpicommon::IamTheMaster())
                return;
            std::cout << '\r';
        }
        else
        {
            std::cout << mpicommon::world.rank << ": ";
            flushOnly = false;
        }
        std::cout << "MPI " << int(mpiDuration * 1000) << "ms | "
                  << "Barrier " << int(barrierDuration * 1000) << "ms | ";
    }
    else
        std::cout << '\r';

    const auto elapsed = _timer.elapsed() + double(_waitTime) / 1e6;
    const auto renderDuration = _api->getEngine().renderDuration;
    const auto overhead = _timer.elapsed() - _api->getEngine().renderDuration;
    std::cout << "encode " << int(encodeDuration * 1000) << "ms | "
              << "render " << int(renderDuration * 1000) << "ms | "
              << "total " << int(elapsed * 1000) << "ms | "
              << "overhead " << int(overhead * 1000) << "ms | " << 1. / elapsed
              << "/" << 1. / renderDuration << " FPS";
    if (flushOnly)
        std::cout << std::flush;
    else
        std::cout << std::endl;
}

void Streamer::_createFrameBuffers()
{
    // in the CPU/ospray case, the OpenDeckPlugin creates the buffers for us
    if (_api->getParametersManager().getApplicationParameters().getEngine() !=
        "optix")
        return;

    auto &engine = _api->getEngine();

    auto format = brayns::FrameBufferFormat::rgba_i8;
    if (useCudaBuffer())
        format = brayns::FrameBufferFormat::none;
    frameBuffers[0] = engine.createFrameBuffer("", {width(), height()}, format);
    engine.addFrameBuffer(frameBuffers[0]);

    if (useCudaBuffer())
        frameBuffers[1] =
            engine.createFrameBuffer("", {width(), height()}, format);
}

brayns::FrameBufferPtr Streamer::_getFrameBuffer() const
{
    const size_t fbIndex = useCudaBuffer() ? 0 : _props.getProperty<int>("fb");
    const auto &frameBuffers = _api->getEngine().getFrameBuffers();
    if (fbIndex >= frameBuffers.size())
        return brayns::FrameBufferPtr();
    return frameBuffers[fbIndex];
}

void Streamer::_swapFrameBuffer()
{
#ifdef USE_CUDA_HACK
    if (!useCudaBuffer() || _fbModified)
        return;

    const auto &frameBuffers = _api->getEngine().getFrameBuffers();
    if (frameBuffers.size() < 1)
        return;
    auto &frameBuffer = frameBuffers[_props.getProperty<int>("fb")];
    frameBuffer->setFormat(brayns::FrameBufferFormat::none);
    _fbModified = true;

    _api->getCamera().setInitialState({0, 0, 0}, {1, 0, 0, 0});
#else
    if (!useCudaBuffer() || !asyncEncode())
        return;

    auto &engine = _api->getEngine();
    engine.removeFrameBuffer(frameBuffers[currentFB]);
    currentFB = 1 - currentFB;
    engine.addFrameBuffer(frameBuffers[currentFB]);
#endif
}

void Streamer::_syncFrame()
{
    if (useMPI())
    {
        brayns::Timer timer;
        timer.start();
        if (mpicommon::IamTheMaster())
            _frameData->serialize(_frameNumber);
        else
            _frameData->deserialize(_frameNumber);
        mpiDuration = timer.elapsed();
    }

    auto &camera = _api->getCamera();
    camera.updateProperty("segmentId", _props.getProperty<int>("segment"),
                          false);

    auto sunLight = _api->getScene().getLightManager().getLight(0);
    auto sun = std::dynamic_pointer_cast<brayns::DirectionalLight>(sunLight);
    if (sun)
    {
        brayns::Transformation transformation;
        transformation.setTranslation(sun->_direction);
        _lightModel->setTransformation(transformation);
        _api->getScene().markModified();
    }

    //    if (camera.isModified())
    //    {
    //        const auto flystickRot =
    //            _api->getCamera().getPropertyOrValue<std::array<double, 4>>(
    //                "flystickRotation", {{0.0, 0.0, 0.0, 1.0}});

    //        auto sunLight = _api->getScene().getLightManager().getLight(0);
    //        auto sun =
    //            std::dynamic_pointer_cast<brayns::DirectionalLight>(sunLight);
    //        if (sun)
    //        {
    //            sun->_direction =
    //                glm::rotate(brayns::Quaterniond(flystickRot[3],
    //                flystickRot[0],
    //                                                flystickRot[1],
    //                                                flystickRot[2]),
    //                            brayns::Vector3d(0, 0, -1));
    //            _api->getScene().getLightManager().addLight(sun);
    //        }
    //    }
}

void Streamer::_nextFrame()
{
    _timer.stop();
    if (isLocalOrMaster())
    {
        _waitTime = std::max(0., (1.0 / fps()) * 1e6 - _timer.microseconds());
        if (_waitTime > 0)
            std::this_thread::sleep_for(std::chrono::microseconds(_waitTime));
        ++_frameNumber;
    }

    _timer.start();
}

void Streamer::_barrier()
{
    if (useMPI())
    {
        brayns::Timer timer;
        timer.start();
        mpicommon::world.barrier();
        barrierDuration = timer.elapsed();
    }
}

void Streamer::_initMPI()
{
    if (!useMPI())
        return;

    _barrier();
    if (mpicommon::IamTheMaster())
    {
        MPI_CALL(Comm_split(mpicommon::world.comm, 1, mpicommon::world.rank,
                            &mpicommon::app.comm));

        mpicommon::app.makeIntraComm();

        MPI_CALL(Intercomm_create(mpicommon::app.comm, 0, mpicommon::world.comm,
                                  1, 1, &mpicommon::worker.comm));

        mpicommon::worker.makeInterComm();
        mpiFabric =
            std::make_unique<mpicommon::MPIBcastFabric>(mpicommon::worker,
                                                        MPI_ROOT, 0);
    }
    else
    {
        MPI_CALL(Comm_split(mpicommon::world.comm, 0, mpicommon::world.rank,
                            &mpicommon::worker.comm));

        mpicommon::worker.makeIntraComm();

        MPI_CALL(Intercomm_create(mpicommon::worker.comm, 0,
                                  mpicommon::world.comm, 0, 1,
                                  &mpicommon::app.comm));

        mpicommon::app.makeInterComm();
        mpiFabric = std::make_unique<mpicommon::MPIBcastFabric>(mpicommon::app,
                                                                MPI_ROOT, 0);
    }
    _frameData = std::make_unique<FrameData>(*mpiFabric, *_api);
    _barrier();
}

Streamer::FrameData::FrameData(ospcommon::networking::Fabric &mpiFabric_,
                               brayns::PluginAPI &api)
    : mpiFabric(mpiFabric_)
    , rp(api.getParametersManager().getRenderingParameters())
    , camera(api.getCamera())
    , scene(api.getScene())
    , renderer(api.getRenderer())
{
}

bool Streamer::FrameData::serialize(const size_t frameNumber) const
{
    ospcommon::networking::BufferedWriteStream stream(mpiFabric);
    stream << frameNumber << rp.isModified() << camera.isModified()
           << scene.isModified() << renderer.isModified();
    if (camera.isModified())
    {
        stream << camera.getTarget() << camera.getPosition()
               << camera.getOrientation() << camera.getPropertyMap();
    }
    if (rp.isModified())
    {
        stream << rp.getCurrentRenderer() << rp.getSamplesPerPixel()
               << rp.getBackgroundColor();
    }
    if (scene.isModified())
        stream << scene.getEnvironmentMap();
    if (renderer.isModified())
        stream << renderer.getPropertyMap();

    auto sunLight = scene.getLightManager().getLight(0);
    auto sun = std::dynamic_pointer_cast<brayns::DirectionalLight>(sunLight);
    stream << sun->_direction;

    stream << scene.getModel(0)->getVisible();
    stream.flush();
    return rp.isModified();
}

bool Streamer::FrameData::deserialize(size_t &frameNumber)
{
    ospcommon::networking::BufferedReadStream stream(mpiFabric);
    bool rpModified, camModified, sceneModified, rendererModified;
    stream >> frameNumber >> rpModified >> camModified >> sceneModified >>
        rendererModified;
    if (camModified)
    {
        brayns::Vector3d target, position;
        brayns::Quaterniond orientation;
        brayns::PropertyMap props;

        stream >> target >> position >> orientation >> props;

        camera.setTarget(target);
        camera.setPosition(position);
        camera.setOrientation(orientation);
        camera.updateProperties(props);
    }
    if (rpModified)
    {
        std::string renderer;
        uint32_t spp;
        brayns::Vector3d bgColor;
        stream >> renderer >> spp >> bgColor;
        rp.setCurrentRenderer(renderer);
        rp.setSamplesPerPixel(spp);
        rp.setBackgroundColor(bgColor);
    }
    if (sceneModified)
    {
        std::string envMap;
        stream >> envMap;
        scene.setEnvironmentMap(envMap);
    }
    if (rendererModified)
    {
        brayns::PropertyMap props;
        stream >> props;
        renderer.updateProperties(props);
    }

    brayns::Vector3d direction;
    stream >> direction;
    auto sunLight = scene.getLightManager().getLight(0);
    auto sun = std::dynamic_pointer_cast<brayns::DirectionalLight>(sunLight);
    sun->_direction = direction;
    scene.getLightManager().addLight(sun);

    bool visi;
    stream >> visi;
    scene.getModel(0)->setVisible(visi);

    return rpModified || camModified || sceneModified;
}
}

extern "C" brayns::ExtensionPlugin *brayns_plugin_create(int argc,
                                                         const char **argv)
{
    brayns::PropertyMap props;
    props.setProperty({"host", std::string("localhost:49990")});
    props.setProperty({"fps", 60});
    props.setProperty({"bitrate", 10, {"Bitrate", "in MBit/s"}});
    props.setProperty({"width", 1920});
    props.setProperty({"height", 1080});
    props.setProperty({"profile", std::string("main")});
    props.setProperty({"fb", 0});
    props.setProperty({"gop", 60});
    props.setProperty({"rtsp", false});
    props.setProperty({"hevc", false});
    props.setProperty({"async-encode", false});
    props.setProperty({"stats", false});
    props.setProperty({"segment", 0});
    props.setProperty({"mpi", false});
    props.setProperty({"master-stats", false});
    if (!props.parse(argc, argv))
        return nullptr;

    if (props.getProperty<bool>("mpi"))
        mpicommon::init(&argc, argv, true);

    return new streamer::Streamer(props);
}
