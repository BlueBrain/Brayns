#pragma once

extern "C"
{
#include <libavcodec/avcodec.h>
}

#include "../Exception.h"
#include "../Status.h"

namespace ffmpeg
{
class ImageDecoder
{
public:
    ImageDecoder(AVCodecContext* context)
        : _context(context)
    {
    }

    void sendPacket(AVPacket* packet)
    {
        Status status = avcodec_send_packet(_context, packet);
        if (!status.isSuccess())
        {
            throw Exception("Cannot send input packet", status);
        }
    }

    void flush()
    {
        Status status = avcodec_send_packet(_context, nullptr);
        if (!status.isSuccess())
        {
            throw Exception("Cannot flush input decoder", status);
        }
    }

    void receiveFrame(AVFrame* frame)
    {
        Status status = avcodec_receive_frame(_context, frame);
        if (!status.isSuccess())
        {
            throw Exception("Cannot receive decoded frame", status);
        }
    }

private:
    AVCodecContext* _context;
};
} // namespace ffmpeg