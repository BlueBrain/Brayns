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
#include <brayns/engine/Renderer.h>
#include <brayns/engine/Scene.h>
#include <brayns/parameters/ParametersManager.h>
#include <brayns/pluginapi/PluginAPI.h>

#include <brayns/common/light/Light.h>

#ifdef USE_MPI
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
#endif

#define THROW(msg)                         \
    {                                      \
        std::stringstream s;               \
        s << msg;                          \
        throw std::runtime_error(s.str()); \
    }

#define STREAM_PIX_FMT AV_PIX_FMT_YUV420P

#define USE_CUDA_HACK

namespace
{
void _copyToImage(streamer::Image &image, brayns::FrameBuffer &frameBuffer)
{
    const auto &size = frameBuffer.getSize();
    const size_t bufferSize = size.x * size.y * frameBuffer.getColorDepth();
    const auto data = frameBuffer.getColorBuffer();

    if (image.data.size() < bufferSize)
        image.data.resize(bufferSize);
    memcpy(image.data.data(), data, bufferSize);
}
}

namespace streamer
{
Streamer::Streamer(const brayns::PropertyMap &props)
    : _props(props)
{
}

Streamer::~Streamer()
{
    if (asyncEncode())
    {
        _images.push(Image{});
        _encodeThread.join();
        if (!useGPU())
        {
            ++_pkts;
            _encodeFinishThread.join();
        }
    }
#ifdef USE_NVPIPE
    if (encoder)
        NvPipe_Destroy(encoder);
#endif

    if (pkt)
        av_packet_free(&pkt);
    if (codecContext)
    {
        avcodec_close(codecContext);
        avcodec_free_context(&codecContext);
    }

    if (streamContext)
    {
        if (streamContext->pb)
            avio_close(streamContext->pb);
        avformat_free_context(streamContext);
    }
    avformat_network_deinit();
}

void Streamer::init()
{
    av_register_all();
    if (avformat_network_init() < 0)
        THROW("Could not init stream network");

    const bool useRTP = !_props.getProperty<bool>("rtsp");
    const bool useHEVC = _props.getProperty<bool>("hevc");
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

    if (useGPU())
    {
        stream->codecpar->codec_id = codecID;
        stream->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
        stream->codecpar->width = width();
        stream->codecpar->height = height();

#ifdef USE_NVPIPE
        encoder = NvPipe_CreateEncoder(NVPIPE_RGBA32,
                                       useHEVC ? NVPIPE_HEVC : NVPIPE_H264,
                                       NVPIPE_LOSSY, bitrate(), fps(), width(),
                                       height());
        if (!encoder)
            THROW("Failed to create encoder: " << NvPipe_GetError(nullptr));
#endif
    }
    else
    {
        codecContext = avcodec_alloc_context3(codec);
        if (!codecContext)
            THROW("Could not allocate video codec context");

        codecContext->codec_tag = 0;
        codecContext->codec_id = codecID;
        codecContext->codec_type = AVMEDIA_TYPE_VIDEO;
        codecContext->width = width();
        codecContext->height = height();
        codecContext->gop_size = gop();
        codecContext->pix_fmt = STREAM_PIX_FMT;
        codecContext->framerate = avFPS;
        codecContext->time_base = av_inv_q(avFPS);
        codecContext->bit_rate = bitrate();
        codecContext->max_b_frames = 0;

        if (streamContext->oformat->flags & AVFMT_GLOBALHEADER)
            codecContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

        if (avcodec_parameters_from_context(stream->codecpar, codecContext) < 0)
            THROW("Could not initialize stream codec parameters!");

        AVDictionary *codec_options = nullptr;
        av_dict_set(&codec_options, "profile", profile().c_str(), 0);
        av_dict_set(&codec_options, "preset", "ultrafast", 0);
        av_dict_set(&codec_options, "tune", "zerolatency", 0);

        if (avcodec_open2(codecContext, codec, &codec_options) < 0)
            THROW("Could not open video encoder!");
        av_dict_free(&codec_options);

        stream->codecpar->extradata_size = codecContext->extradata_size;
        stream->codecpar->extradata =
            static_cast<uint8_t *>(av_mallocz(codecContext->extradata_size));
        memcpy(stream->codecpar->extradata, codecContext->extradata,
               codecContext->extradata_size);

        picture.init(codecContext->pix_fmt, width(), height());
    }

    if (avformat_write_header(streamContext, nullptr) !=
        AVSTREAM_INIT_IN_WRITE_HEADER)
    {
        THROW("Could not write stream header")
    }

#ifdef USE_MPI
    _initMPI();
#endif

    if (useRTP)
    {
        char buf[200000];
        AVFormatContext *ac[] = {streamContext};
        av_sdp_create(ac, 1, buf, 20000);

        std::stringstream outFile;
        outFile << "/tmp/test";
#ifdef USE_MPI
        if (useMPI())
            outFile << mpicommon::world.rank;
#endif
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
        if (!useGPU())
            _encodeFinishThread =
                std::thread(std::bind(&Streamer::_runAsyncEncodeFinish, this));
    }

#ifndef USE_CUDA_HACK
    _createFrameBuffers();
#endif

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

    frameBuffer->map();

#ifdef USE_CUDA_HACK
    if (asyncEncode() && useCudaBuffer())
        _images.waitSize(0);
    const void *buffer = useCudaBuffer() ? frameBuffer->cudaBuffer()
                                         : frameBuffer->getColorBuffer();
#else
    const void *buffer = frameBuffer->getColorBuffer();
#endif
    if (asyncEncode())
    {
        if (useGPU())
        {
            Image img(_frameNumber, frameBuffer->getSize());
            img.buffer = buffer;
            _images.push(img);
        }
        else
        {
            if (asyncCopy())
            {
                Image img(_frameNumber, frameBuffer->getSize());
                _copyToImage(img, *frameBuffer);
                _images.push(img);
            }
            else
                encodeFrame(_frameNumber, frameBuffer->getSize(), buffer);
        }
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

    if (useGPU())
    {
        static std::vector<uint8_t> compressed;
        const size_t bufSize = size.x * size.y * 4;
        if (compressed.size() < bufSize)
            compressed.resize(bufSize);

        const auto compressedSize =
#ifdef USE_NVPIPE
            NvPipe_Encode(encoder, data, size.x * 4, compressed.data(),
                          compressed.size(), size.x, size.y,
                          gop() > 0 ? frameNumber % gop() == 0 : false);
#else
            0;
#endif
        av_init_packet(pkt);
        pkt->data = compressed.data();
        pkt->size = compressedSize;
        pkt->pts = pts;
        pkt->dts = pts;
        pkt->stream_index = stream->index;

        if (!memcmp(compressed.data(), "\x00\x00\x00\x01\x67", 5))
            pkt->flags |= AV_PKT_FLAG_KEY;
    }
    else
    {
        sws_context =
            sws_getCachedContext(sws_context, size.x, size.y, AV_PIX_FMT_RGBA,
                                 width(), height(), STREAM_PIX_FMT,
                                 SWS_FAST_BILINEAR, 0, 0, 0);
        const int stride[] = {4 * (int)size.x};
        auto cdata = reinterpret_cast<const uint8_t *const>(data);
        sws_scale(sws_context, &cdata, stride, 0, size.y, picture.frame->data,
                  picture.frame->linesize);
        picture.frame->pts = pts;

        if (avcodec_send_frame(codecContext, picture.frame) < 0)
            return;
    }

    encodeDuration = encodeTimer.elapsed();

    if (asyncEncode() && !useGPU())
        ++_pkts;
    else
        streamFrame(!useGPU());
}

void Streamer::streamFrame(const bool finishEncode)
{
    if (finishEncode)
    {
        brayns::Timer timer;
        timer.start();
        const auto ret = avcodec_receive_packet(codecContext, pkt);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            return;
        // encodeDuration += timer.elapsed();
        for (double g = encodeDuration;
             !encodeDuration.compare_exchange_strong(g, g + timer.elapsed());)
            ;
    }

    _barrier();
#if 0
    av_write_frame(streamContext, pkt);
    av_write_frame(streamContext, nullptr);
#else
    av_interleaved_write_frame(streamContext, pkt);
#endif
    av_packet_unref(pkt);
}

bool Streamer::useGPU() const
{
#ifdef USE_NVPIPE
    return _props.getProperty<bool>("gpu");
#else
    return false;
#endif
}

bool Streamer::useMPI() const
{
#ifdef USE_MPI
    return _props.getProperty<bool>("mpi");
#else
    return false;
#endif
}

bool Streamer::useCudaBuffer() const
{
    return useGPU() &&
           _api->getParametersManager()
                   .getApplicationParameters()
                   .getEngine() == "optix";
}

bool Streamer::isLocalOrMaster() const
{
#ifdef USE_MPI
    return !useMPI() || mpicommon::IamTheMaster();
#else
    return true;
#endif
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

void Streamer::_runAsyncEncodeFinish()
{
    while (_api->getEngine().getKeepRunning())
    {
        _pkts.waitGT(0);
        streamFrame();
        --_pkts;
    }
}

void Streamer::printStats()
{
    bool flushOnly = true;

#ifdef USE_MPI
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
#endif
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
#ifdef USE_MPI
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
#endif

    auto &camera = _api->getCamera();
    camera.updateProperty("segmentId", _props.getProperty<int>("segment"),
                          false);

    if (camera.isModified())
    {
        const auto flystickRot =
            _api->getCamera().getPropertyOrValue<std::array<double, 4>>(
                "flystickRotation", {{0.0, 0.0, 0.0, 1.0}});

        auto sunLight = _api->getScene().getLightManager().getLight(0);
        auto sun =
            std::dynamic_pointer_cast<brayns::DirectionalLight>(sunLight);
        if (sun)
        {
            sun->_direction =
                glm::rotate(brayns::Quaterniond(flystickRot[3], flystickRot[0],
                                                flystickRot[1], flystickRot[2]),
                            brayns::Vector3d(0, 0, -1));
            _api->getScene().getLightManager().addLight(sun);
        }
    }
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
#ifdef USE_MPI
    if (useMPI())
    {
        brayns::Timer timer;
        timer.start();
        mpicommon::world.barrier();
        barrierDuration = timer.elapsed();
    }
#endif
}

#ifdef USE_MPI
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

    return rpModified || camModified || sceneModified;
}
#endif
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
    props.setProperty({"async-copy", false}); // CPU only (sws_scale)
    props.setProperty({"stats", false});
    props.setProperty({"segment", 0});
#ifdef USE_NVPIPE
    props.setProperty({"gpu", false});
#endif
#ifdef USE_MPI
    props.setProperty({"mpi", false});
    props.setProperty({"master-stats", false});
#endif
    if (!props.parse(argc, argv))
        return nullptr;

#ifdef USE_MPI
    if (props.getProperty<bool>("mpi"))
        mpicommon::init(&argc, argv, true);
#endif

    return new streamer::Streamer(props);
}
