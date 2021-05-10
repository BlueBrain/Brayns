#pragma once

extern "C"
{
#include <libswscale/swscale.h>
}

#include "../Exception.h"
#include "../Status.h"
#include "../common/Frame.h"
#include "ConversionContext.h"
#include "ConversionInfo.h"

namespace ffmpeg
{
class ImageConverter
{
public:
    static FramePtr convert(const AVFrame* frame, const ConversionInfo& info)
    {
        auto context = _createContext(frame, info);
        auto newFrame = Frame::create();
        _setupNewFrame(newFrame.get(), info);
        _convert(context.get(), frame, newFrame.get());
        return newFrame;
    }

private:
    static ConversionContextPtr _createContext(const AVFrame* frame,
                                               const ConversionInfo& info)
    {
        auto frameInfo = ConversionInfo::fromFrame(frame);
        return ConversionContext::create(frameInfo, info);
    }

    static void _setupNewFrame(AVFrame* frame, const ConversionInfo& info)
    {
        info.toFrame(frame);
        Status status = av_frame_get_buffer(frame, 0);
        if (!status.isSuccess())
        {
            throw Exception("Cannot setup new frame", status);
        }
    }

    static void _convert(SwsContext* context, const AVFrame* source,
                         AVFrame* destination)
    {
        sws_scale(context, source->data, source->linesize, 0, source->height,
                  destination->data, destination->linesize);
    }
};
} // namespace ffmpeg