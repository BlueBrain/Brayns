#pragma once

#include <utility>

extern "C"
{
#include <libavformat/avformat.h>
}

namespace ffmpeg
{
class FormatContextPtr
{
public:
    FormatContextPtr() = default;

    explicit FormatContextPtr(AVFormatContext* context)
        : _context(context)
    {
    }

    ~FormatContextPtr()
    {
        if (_context)
        {
            avformat_free_context(_context);
        }
    }

    FormatContextPtr(const FormatContextPtr&) = delete;

    FormatContextPtr(FormatContextPtr&& other)
        : _context(std::exchange(other._context, nullptr))
    {
    }

    FormatContextPtr& operator=(const FormatContextPtr&) = delete;

    FormatContextPtr& operator=(FormatContextPtr&& other)
    {
        std::swap(_context, other._context);
        return *this;
    }

    AVFormatContext* get() const { return _context; }

    AVFormatContext* operator->() const { return _context; }

private:
    AVFormatContext* _context = nullptr;
};
} // namespace ffmpeg