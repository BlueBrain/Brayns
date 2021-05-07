#pragma once

#include <string>

extern "C"
{
#include <libavformat/avformat.h>
}

#include "../Exception.h"
#include "../Status.h"
#include "../common/FormatContextPtr.h"

namespace ffmpeg
{
class OutputContext
{
public:
    static FormatContextPtr create(const std::string& filename)
    {
        AVFormatContext* context = nullptr;
        Status status =
            avformat_alloc_output_context2(&context, nullptr, nullptr,
                                           filename.c_str());
        if (!status.isSuccess())
        {
            throw Exception("Cannot allocate output context", status);
        }
        return FormatContextPtr(context);
    }
};
} // namespace ffmpeg