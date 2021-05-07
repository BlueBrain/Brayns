#pragma once

extern "C"
{
#include <libavutil/frame.h>
}

namespace ffmpeg
{
struct ConversionInfo
{
    int width = 0;
    int height = 0;
    AVPixelFormat format = AV_PIX_FMT_NONE;

    static ConversionInfo fromFrame(const AVFrame* frame)
    {
        ConversionInfo info;
        info.width = frame->width;
        info.height = frame->height;
        info.format = AVPixelFormat(frame->format);
        return info;
    }

    void toFrame(AVFrame* frame) const
    {
        frame->width = width;
        frame->height = height;
        frame->format = format;
    }
};
} // namespace ffmpeg