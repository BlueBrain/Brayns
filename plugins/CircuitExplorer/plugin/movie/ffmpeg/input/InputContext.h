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
class InputContext
{
public:
    static FormatContextPtr create(const std::string& filename)
    {
        auto context = _openInput(filename);
        _findStreamInfo(context.get());
        return context;
    }

private:
    static FormatContextPtr _openInput(const std::string& filename)
    {
        AVFormatContext* context = nullptr;
        Status status =
            avformat_open_input(&context, filename.c_str(), nullptr, nullptr);
        if (!status.isSuccess())
        {
            throw Exception("Cannot open input context", status);
        }
        return FormatContextPtr(context);
    }

    static void _findStreamInfo(AVFormatContext* context)
    {
        Status status = avformat_find_stream_info(context, nullptr);
        if (!status.isSuccess())
        {
            throw Exception("Cannot find input stream info", status);
        }
    }
};
} // namespace ffmpeg