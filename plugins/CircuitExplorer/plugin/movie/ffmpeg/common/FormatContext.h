#pragma once

#include <memory>
#include <utility>

extern "C"
{
#include <libavformat/avformat.h>
}

namespace ffmpeg
{
class FormatContext
{
public:
    struct Deleter
    {
        void operator()(AVFormatContext* context) const
        {
            avformat_free_context(context);
        }
    };

    using Ptr = std::unique_ptr<AVFormatContext, Deleter>;
};

using FormatContextPtr = FormatContext::Ptr;
} // namespace ffmpeg