#pragma once

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
}

#include "../Exception.h"
#include "../common/CodecContextPtr.h"
#include "EncoderInfo.h"

namespace ffmpeg
{
class Encoder
{
public:
    static CodecContextPtr create(const EncoderInfo& info)
    {
        auto context = CodecContextPtr::create(info.codec);
        _setup(context.get(), info.stream->codecpar);
        _loadInfo(context.get(), info);
        _openContext(context.get(), info.codec);
        _updateStream(context.get(), info.stream);
        return context;
    }

private:
    static void _setup(AVCodecContext* context, AVCodecParameters* parameters)
    {
        Status status = avcodec_parameters_to_context(context, parameters);
        if (!status.isSuccess())
        {
            throw Exception("Cannot setup encoder context", status);
        }
    }

    static void _loadInfo(AVCodecContext* context, const EncoderInfo& info)
    {
        context->time_base = {1, info.framerate};
        if (info.stream->codecpar->codec_id == AV_CODEC_ID_H264)
        {
            av_opt_set(context->priv_data, "preset", "ultrafast", 0);
        }
        if (info.context->oformat->flags & AVFMT_GLOBALHEADER)
        {
            context->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
        }
    }

    static void _openContext(AVCodecContext* context, AVCodec* codec)
    {
        Status status = avcodec_open2(context, codec, nullptr);
        if (!status.isSuccess())
        {
            throw Exception("Cannot open encoder", status);
        }
    }

    static void _updateStream(AVCodecContext* context, AVStream* stream)
    {
        Status status =
            avcodec_parameters_from_context(stream->codecpar, context);
        if (!status.isSuccess())
        {
            throw Exception("Cannot setup video stream", status);
        }
    }
};
} // namespace ffmpeg