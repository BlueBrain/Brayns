/* Copyright (c) 2021, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Adrien Fleury <adrien.fleury@epfl.ch>
 *
 * This file is part of Brayns <https://github.com/BlueBrain/Brayns>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

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