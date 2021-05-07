#pragma once

#include <utility>

extern "C"
{
#include <libavcodec/avcodec.h>
}

#include "../Exception.h"

namespace ffmpeg
{
class CodecContextPtr
{
public:
    static CodecContextPtr create(AVCodec* codec)
    {
        auto context = avcodec_alloc_context3(codec);
        if (!context)
        {
            throw Exception("Cannot allocate codec context");
        }
        return CodecContextPtr(context);
    }

    CodecContextPtr() = default;

    explicit CodecContextPtr(AVCodecContext* context)
        : _context(context)
    {
    }

    ~CodecContextPtr()
    {
        if (_context)
        {
            avcodec_free_context(&_context);
        }
    }

    CodecContextPtr(const CodecContextPtr&) = delete;

    CodecContextPtr(CodecContextPtr&& other)
        : _context(std::exchange(other._context, nullptr))
    {
    }

    CodecContextPtr& operator=(const CodecContextPtr&) = delete;

    CodecContextPtr& operator=(CodecContextPtr&& other)
    {
        std::swap(_context, other._context);
        return *this;
    }

    AVCodecContext* get() const { return _context; }

    AVCodecContext* operator->() const { return _context; }

private:
    AVCodecContext* _context = nullptr;
};
} // namespace ffmpeg