#pragma once

#include <utility>

extern "C"
{
#include <libavutil/frame.h>
}

#include "../Exception.h"

namespace ffmpeg
{
class FramePtr
{
public:
    static FramePtr create()
    {
        auto frame = av_frame_alloc();
        if (!frame)
        {
            throw Exception("Cannot allocate frame");
        }
        return FramePtr(frame);
    }

    FramePtr() = default;

    explicit FramePtr(AVFrame* context)
        : _frame(context)
    {
    }

    ~FramePtr()
    {
        if (_frame)
        {
            av_frame_free(&_frame);
        }
    }

    FramePtr(const FramePtr&) = delete;

    FramePtr(FramePtr&& other)
        : _frame(std::exchange(other._frame, nullptr))
    {
    }

    FramePtr& operator=(const FramePtr&) = delete;

    FramePtr& operator=(FramePtr&& other)
    {
        std::swap(_frame, other._frame);
        return *this;
    }

    AVFrame* get() const { return _frame; }

    AVFrame* operator->() const { return _frame; }

private:
    AVFrame* _frame = nullptr;
};
} // namespace ffmpeg