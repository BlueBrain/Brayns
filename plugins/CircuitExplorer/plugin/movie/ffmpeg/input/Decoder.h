#pragma once

extern "C"
{
#include <libavcodec/avcodec.h>
}

#include "../Exception.h"
#include "../common/CodecContextPtr.h"

namespace ffmpeg
{
class Decoder
{
public:
    static CodecContextPtr create(AVCodecParameters* parameters)
    {
        auto codec = _findDecoder(parameters);
        auto context = CodecContextPtr::create(codec);
        _setupContext(context.get(), parameters);
        _openContext(context.get(), codec);
        return context;
    }

private:
    static AVCodec* _findDecoder(AVCodecParameters* parameters)
    {
        auto codec = avcodec_find_decoder(parameters->codec_id);
        if (!codec)
        {
            throw Exception("Cannot find decoder");
        }
        return codec;
    }

    static void _setupContext(AVCodecContext* context,
                              AVCodecParameters* parameters)
    {
        Status status = avcodec_parameters_to_context(context, parameters);
        if (!status.isSuccess())
        {
            throw Exception("Cannot fill decoder context", status);
        }
    }

    static void _openContext(AVCodecContext* context, AVCodec* codec)
    {
        Status status = avcodec_open2(context, codec, nullptr);
        if (!status.isSuccess())
        {
            throw Exception("Cannot open decoder", status);
        }
    }
};
} // namespace ffmpeg