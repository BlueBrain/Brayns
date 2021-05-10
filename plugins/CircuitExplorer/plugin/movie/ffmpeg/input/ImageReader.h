#pragma once

#include <string>

#include "../common/Frame.h"
#include "../common/Packet.h"
#include "Decoder.h"
#include "ImageDecoder.h"
#include "InputContext.h"
#include "InputVideoStream.h"

namespace ffmpeg
{
class ImageReader
{
public:
    static FramePtr read(const std::string& filename)
    {
        auto context = InputContext::create(filename);
        auto codec = _createCodec(context.get());
        auto packet = _readRawImage(context.get());
        auto frame = _decodeImage(codec.get(), packet.get());
        return frame;
    }

private:
    static CodecContextPtr _createCodec(AVFormatContext* context)
    {
        auto codec = InputVideoStream::findCodec(context);
        return Decoder::create(codec);
    }

    static PacketPtr _readRawImage(AVFormatContext* context)
    {
        auto packet = Packet::create();
        _readPacket(context, packet.get());
        return packet;
    }

    static void _readPacket(AVFormatContext* context, AVPacket* packet)
    {
        Status status = av_read_frame(context, packet);
        if (!status.isSuccess())
        {
            throw Exception("Cannot read input packet", status);
        }
    }

    static FramePtr _decodeImage(AVCodecContext* codec, AVPacket* packet)
    {
        ImageDecoder decoder(codec);
        decoder.sendPacket(packet);
        decoder.flush();
        auto frame = Frame::create();
        decoder.receiveFrame(frame.get());
        return frame;
    }
};
} // namespace ffmpeg