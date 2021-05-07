#pragma once

#include <utility>

extern "C"
{
#include <libswscale/swscale.h>
}

#include "../Exception.h"
#include "ConversionInfo.h"

namespace ffmpeg
{
class ConversionContextPtr
{
public:
    static ConversionContextPtr create(const ConversionInfo& source,
                                       const ConversionInfo& destination)
    {
        auto context = sws_getContext(source.width, source.height,
                                      source.format, destination.width,
                                      destination.height, destination.format,
                                      SWS_BILINEAR, nullptr, nullptr, nullptr);
        if (!context)
        {
            throw Exception("Cannot allocate conversion context");
        }
        return ConversionContextPtr(context);
    }

    ConversionContextPtr() = default;

    explicit ConversionContextPtr(SwsContext* context)
        : _context(context)
    {
    }

    ~ConversionContextPtr()
    {
        if (_context)
        {
            sws_freeContext(_context);
        }
    }

    ConversionContextPtr(const ConversionContextPtr&) = delete;

    ConversionContextPtr(ConversionContextPtr&& other)
        : _context(std::exchange(other._context, nullptr))
    {
    }

    ConversionContextPtr& operator=(const ConversionContextPtr&) = delete;

    ConversionContextPtr& operator=(ConversionContextPtr&& other)
    {
        std::swap(_context, other._context);
        return *this;
    }

    operator SwsContext*() const { return _context; }

    SwsContext* operator->() const { return _context; }

private:
    SwsContext* _context = nullptr;
};
} // namespace ffmpeg