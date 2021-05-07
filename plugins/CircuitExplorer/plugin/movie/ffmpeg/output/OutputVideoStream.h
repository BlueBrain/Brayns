#pragma once

extern "C"
{
#include <libavformat/avformat.h>
}

#include "../Exception.h"
#include "../VideoInfo.h"

namespace ffmpeg
{
class OutputVideoStream
{
public:
    static AVStream* add(AVFormatContext* context, const VideoInfo& info)
    {
        auto stream = _addStream(context, info.codec);
        _setupParameters(stream, info);
        return stream;
    }

private:
    static AVStream* _addStream(AVFormatContext* context, AVCodec* codec)
    {
        auto stream = avformat_new_stream(context, codec);
        if (!stream)
        {
            throw Exception("Cannot create output video stream");
        }
        return stream;
    }

    static void _setupParameters(AVStream* stream, const VideoInfo& info)
    {
        stream->codecpar->codec_id = info.codec->id;
        stream->codecpar->codec_type = info.codec->type;
        stream->codecpar->width = info.width;
        stream->codecpar->height = info.height;
        stream->codecpar->format = info.format;
        stream->codecpar->bit_rate = info.bitrate;
        stream->time_base = {1, info.framerate};
    }
};
} // namespace ffmpeg