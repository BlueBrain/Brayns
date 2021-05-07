#pragma once

extern "C"
{
#include <libavcodec/avcodec.h>
}

#include "../Exception.h"
#include "../Status.h"

namespace ffmpeg
{
class VideoEncoder
{
public:
    void open(AVCodecContext* context) { _context = context; }

    void sendFrame(const AVFrame* frame)
    {
        Status status = avcodec_send_frame(_context, frame);
        if (!status.isSuccess())
        {
            throw Exception("Cannot send output frame", status);
        }
    }

    void flush()
    {
        Status status = avcodec_send_frame(_context, nullptr);
        if (!status.isSuccess())
        {
            throw Exception("Cannot flush output decoder", status);
        }
    }

    bool receivePacket(AVPacket* packet)
    {
        Status status = avcodec_receive_packet(_context, packet);
        if (status.isRetry())
        {
            return false;
        }
        if (status.isError())
        {
            throw Exception("Cannot receive output packet", status);
        }
        return true;
    }

private:
    AVCodecContext* _context = nullptr;
};
} // namespace ffmpeg