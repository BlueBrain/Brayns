#pragma once

#include <memory>
#include <utility>

extern "C"
{
#include <libavutil/frame.h>
}

#include "../Exception.h"

namespace ffmpeg
{
class Frame
{
public:
    struct Deleter
    {
        void operator()(AVFrame* frame) const { av_frame_free(&frame); }
    };

    using Ptr = std::unique_ptr<AVFrame, Deleter>;

    static Ptr create()
    {
        auto frame = av_frame_alloc();
        if (!frame)
        {
            throw Exception("Cannot allocate frame");
        }
        return Ptr(frame);
    }
};

using FramePtr = Frame::Ptr;
} // namespace ffmpeg