/* Copyright (c) 2015-2021 EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 *
 * Responsible Author: adrien.fleury@epfl.ch
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

#include <cassert>
#include <cstring>

#include "Image.h"

namespace brayns
{
class ImageConverter
{
public:
    static Image convertToRgba(const Image &source)
    {
        auto channelCount = source.getChannelCount();
        if (channelCount == 4)
        {
            return source;
        }
        assert(channelCount == 3);
        auto info = source.getInfo();
        info.channelCount = 4;
        Image destination(info, char(-1));
        auto pixelSize = source.getPixelSize();
        for (size_t y = 0; y < info.height; ++y)
        {
            for (size_t x = 0; x < info.width; ++x)
            {
                auto from = source.getData(x, y);
                auto to = destination.getData(x, y);
                std::memcpy(to, from, pixelSize);
            }
        }
        return destination;
    }
};
} // namespace brayns
