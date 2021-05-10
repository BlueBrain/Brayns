#pragma once

#include <memory>
#include <utility>

extern "C"
{
#include <libswscale/swscale.h>
}

#include "../Exception.h"
#include "ConversionInfo.h"

namespace ffmpeg
{
class ConversionContext
{
public:
    struct Deleter
    {
        void operator()(SwsContext* context) const { sws_freeContext(context); }
    };

    using Ptr = std::unique_ptr<SwsContext, Deleter>;

    static Ptr create(const ConversionInfo& source,
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
        return Ptr(context);
    }
};

using ConversionContextPtr = ConversionContext::Ptr;
} // namespace ffmpeg