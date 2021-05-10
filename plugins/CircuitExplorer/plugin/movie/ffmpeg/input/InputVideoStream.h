#pragma once

extern "C"
{
#include <libavformat/avformat.h>
}

#include "../Exception.h"

namespace ffmpeg
{
class InputVideoStream
{
public:
    static AVCodecParameters* findCodec(AVFormatContext* context)
    {
        return find(context)->codecpar;
    }

    static AVStream* find(AVFormatContext* context)
    {
        auto stream = tryFind(context);
        if (!stream)
        {
            throw Exception("Cannot find video stream");
        }
        return stream;
    }

    static AVStream* tryFind(AVFormatContext* context)
    {
        for (int i = 0; i < context->nb_streams; ++i)
        {
            auto stream = context->streams[i];
            if (stream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
            {
                return stream;
            }
        }
        return nullptr;
    }
};
} // namespace ffmpeg