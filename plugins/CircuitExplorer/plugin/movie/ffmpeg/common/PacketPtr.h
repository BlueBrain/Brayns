#pragma once

#include <utility>

extern "C"
{
#include <libavcodec/avcodec.h>
}

#include "../Exception.h"

namespace ffmpeg
{
class PacketPtr
{
public:
    static PacketPtr create()
    {
        auto packet = av_packet_alloc();
        if (!packet)
        {
            throw Exception("Cannot allocate packet");
        }
        return PacketPtr(packet);
    }

    PacketPtr() = default;

    explicit PacketPtr(AVPacket* context)
        : _packet(context)
    {
    }

    ~PacketPtr()
    {
        if (_packet)
        {
            av_packet_free(&_packet);
        }
    }

    PacketPtr(const PacketPtr&) = delete;

    PacketPtr(PacketPtr&& other)
        : _packet(std::exchange(other._packet, nullptr))
    {
    }

    PacketPtr& operator=(const PacketPtr&) = delete;

    PacketPtr& operator=(PacketPtr&& other)
    {
        std::swap(_packet, other._packet);
        return *this;
    }

    AVPacket* get() const { return _packet; }

    AVPacket* operator->() const { return _packet; }

private:
    AVPacket* _packet = nullptr;
};
} // namespace ffmpeg