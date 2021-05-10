#pragma once

#include <memory>
#include <utility>

extern "C"
{
#include <libavcodec/avcodec.h>
}

#include "../Exception.h"

namespace ffmpeg
{
class Packet
{
public:
    struct Deleter
    {
        void operator()(AVPacket* packet) const { av_packet_free(&packet); }
    };

    using Ptr = std::unique_ptr<AVPacket, Deleter>;

    static Ptr create()
    {
        auto packet = av_packet_alloc();
        if (!packet)
        {
            throw Exception("Cannot allocate packet");
        }
        return Ptr(packet);
    }
};

using PacketPtr = Packet::Ptr;
} // namespace ffmpeg