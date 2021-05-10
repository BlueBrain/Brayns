#pragma once

#include <memory>
#include <utility>

extern "C"
{
#include <libavcodec/avcodec.h>
}

#include "../Exception.h"

namespace ffmpeg
{
class CodecContext
{
public:
    struct Deleter
    {
        void operator()(AVCodecContext* context) const
        {
            avcodec_free_context(&context);
        }
    };

    using Ptr = std::unique_ptr<AVCodecContext, Deleter>;

    static Ptr create(AVCodec* codec)
    {
        auto context = avcodec_alloc_context3(codec);
        if (!context)
        {
            throw Exception("Cannot allocate codec context");
        }
        return Ptr(context);
    }
};

using CodecContextPtr = CodecContext::Ptr;
} // namespace ffmpeg