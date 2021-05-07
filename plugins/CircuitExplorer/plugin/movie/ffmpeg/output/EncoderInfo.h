#pragma once

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

namespace ffmpeg
{
struct EncoderInfo
{
    AVFormatContext* context = nullptr;
    AVCodec* codec = nullptr;
    AVStream* stream = nullptr;
    int framerate = 0;
};
} // namespace ffmpeg