#pragma once

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/pixdesc.h>
}

#include "Exception.h"

namespace ffmpeg
{
struct VideoInfo
{
    AVCodec* codec = nullptr;
    int width = 0;
    int height = 0;
    AVPixelFormat format = AV_PIX_FMT_NONE;
    int64_t bitrate = 0;
    int framerate = 0;

    static AVCodec* getCodec(const std::string& name)
    {
        auto codec = avcodec_find_encoder_by_name(name.c_str());
        if (!codec)
        {
            throw Exception("Invalid codec '" + name + "'");
        }
        return codec;
    }

    static AVPixelFormat getFormat(const std::string& name)
    {
        auto format = av_get_pix_fmt(name.c_str());
        if (format == AV_PIX_FMT_NONE)
        {
            throw Exception("Invalid pixel format: '" + name + "'");
        }
        return format;
    }
};
} // namespace ffmpeg