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

#pragma once

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/channel_layout.h>
#include <libavutil/common.h>
#include <libavutil/imgutils.h>
#include <libavutil/mathematics.h>
#include <libavutil/opt.h>
#include <libavutil/samplefmt.h>

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

#include <atomic>
#include <condition_variable>
#include <queue>
#include <string>
#include <thread>

#include <brayns/common/PropertyMap.h>
#include <brayns/common/Timer.h>
#include <brayns/common/types.h>
#include <brayns/pluginapi/ExtensionPlugin.h>

#ifdef USE_MPI
#include <ospray/mpiCommon/MPIBcastFabric.h>
#endif

#ifdef USE_NVPIPE
#include <NvPipe.h>
#endif

namespace streamer
{
class Picture
{
public:
    AVFrame *frame{nullptr};

    int init(enum AVPixelFormat pix_fmt, int width, int height)
    {
        frame = av_frame_alloc();
        frame->format = pix_fmt;
        frame->width = width;
        frame->height = height;
        return av_frame_get_buffer(frame, 32);
    }

    ~Picture()
    {
        if (frame)
            av_frame_free(&frame);
    }
};

struct Image
{
    Image() = default;
    Image(const size_t frameNumber_, const brayns::Vector2ui &size_)
        : frameNumber(frameNumber_)
        , size(size_)
    {
    }
    const size_t frameNumber{0};
    const brayns::Vector2ui size;
    std::vector<char> data;
    const void *buffer{nullptr};
};

template <typename T, size_t S = 2>
class MTQueue
{
public:
    explicit MTQueue(const size_t maxSize = S)
        : _maxSize(maxSize)
    {
    }

    void push(const T &element)
    {
        std::unique_lock<std::mutex> lock(_mutex);
        _condition.wait(lock, [&] { return _queue.size() < _maxSize; });
        _queue.push(element);
        _condition.notify_all();
    }

    T pop()
    {
        std::unique_lock<std::mutex> lock(_mutex);
        _condition.wait(lock, [&] { return !_queue.empty(); });

        T element = _queue.front();
        _queue.pop();
        _condition.notify_all();
        return element;
    }
    size_t size() const
    {
        std::unique_lock<std::mutex> lock(_mutex);
        return _queue.size();
    }

    size_t waitSize(const size_t minSize) const
    {
        std::unique_lock<std::mutex> lock(_mutex);
        _condition.wait(lock, [&] { return _queue.size() >= minSize; });
        return _queue.size();
    }

private:
    std::queue<T> _queue;
    mutable std::mutex _mutex;
    mutable std::condition_variable _condition;
    const size_t _maxSize;
};

template <class T>
class Monitor
{
public:
    Monitor()
        : _value(T(0))
    {
    }

    Monitor &operator++()
    {
        std::unique_lock<std::mutex> lock(_mutex);
        ++_value;
        _condition.notify_all();
        return *this;
    }

    Monitor &operator--()
    {
        std::unique_lock<std::mutex> lock(_mutex);
        --_value;
        _condition.notify_all();
        return *this;
    }

    const T waitGT(const T &value) const
    {
        return _wait([&] { return _value > value; });
    }

private:
    T _value;
    mutable std::mutex _mutex;
    mutable std::condition_variable _condition;

    template <typename F>
    const T _wait(const F &predicate) const
    {
        std::unique_lock<std::mutex> lock(_mutex);
        _condition.wait(lock, predicate);
        return _value;
    }
};

class Streamer : public brayns::ExtensionPlugin
{
public:
    Streamer(const brayns::PropertyMap &props);
    ~Streamer();
    void init() final;
    void preRender() final;
    void postRender() final;

private:
    auto width() const { return _props.getProperty<int>("width"); }
    auto height() const { return _props.getProperty<int>("height"); }
    auto fps() const { return _props.getProperty<int>("fps"); }
    auto bitrate() const { return _props.getProperty<int>("bitrate") * 1e6; }
    auto gop() const { return _props.getProperty<int>("gop"); }
    auto profile() const { return _props.getProperty<std::string>("profile"); }
    auto asyncEncode() const
    {
        return _props.getProperty<bool>("async-encode");
    }
    auto asyncCopy() const { return _props.getProperty<bool>("async-copy"); }
    bool useGPU() const;
    bool useMPI() const;
    bool useCudaBuffer() const;
    bool isLocalOrMaster() const;

    void _runAsyncEncode();
    void _runAsyncEncodeFinish();
    void encodeFrame(const size_t frameNumber, const brayns::Vector2ui &size,
                     const void *data);
    void streamFrame(const bool finishEncode = true);
    void _syncFrame();
    void _barrier();
    void _nextFrame();
    void printStats();
    void _createFrameBuffers();
    brayns::FrameBufferPtr _getFrameBuffer() const;
    void _swapFrameBuffer();

    AVFormatContext *streamContext{nullptr};
    AVStream *stream{nullptr};

    AVCodecContext *codecContext{nullptr};
    AVCodec *codec{nullptr};

    AVPacket *pkt{nullptr};
    SwsContext *sws_context{nullptr};
    Picture picture;

    brayns::Timer _timer;
    int64_t _waitTime{0};

    std::thread _encodeThread;
    std::thread _encodeFinishThread; // CPU only
    MTQueue<Image> _images;
    Monitor<int> _pkts;

#ifdef USE_NVPIPE
    NvPipe *encoder{nullptr};
#endif
    bool _fbModified{false};
    brayns::FrameBufferPtr frameBuffers[2];
    int currentFB{0};

    const brayns::PropertyMap _props;
    size_t _frameNumber{0};
    std::atomic<double> encodeDuration{0};
#ifdef USE_MPI
    void _initMPI();
    double mpiDuration{0};
    double barrierDuration{0};

    std::unique_ptr<ospcommon::networking::Fabric> mpiFabric;

    struct FrameData
    {
        FrameData(ospcommon::networking::Fabric &mpiFabric_,
                  brayns::PluginAPI &api);
        bool serialize(const size_t frameNumber) const;
        bool deserialize(size_t &frameNumber);
        ospcommon::networking::Fabric &mpiFabric;
        brayns::RenderingParameters &rp;
        brayns::Camera &camera;
        brayns::Scene &scene;
        brayns::Renderer &renderer;
    };
    std::unique_ptr<FrameData> _frameData;
#endif
};
}
