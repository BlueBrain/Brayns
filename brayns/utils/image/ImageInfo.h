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

#include <cstddef>

namespace brayns
{
struct ImageInfo
{
    size_t width = 0;
    size_t height = 0;
    size_t channelCount = 0;
    size_t channelSize = 0;

    size_t getSize() const { return getPixelCount() * getPixelSize(); }

    size_t getRowCount() const { return height; }

    size_t getRowSize() const { return width * getPixelSize(); }

    size_t getRowOffset(size_t index) const { return index * getRowSize(); }

    size_t getPixelCount() const { return width * height; }

    size_t getPixelSize() const { return channelCount * channelSize; }

    size_t getPixelIndex(size_t x, size_t y) const { return x + y * width; }

    size_t getPixelOffset(size_t x, size_t y) const
    {
        return getPixelIndex(x, y) * getPixelSize();
    }

    bool isGrey() const { return channelCount == 1; }

    bool isGreyAlpha() const { return channelCount == 2; }

    bool isRgb() const { return channelCount == 3; }

    bool isRgba() const { return channelCount == 4; }

    bool operator==(const ImageInfo &other) const
    {
        return width == other.width && height == other.height &&
               channelCount == other.channelCount &&
               channelSize == other.channelSize;
    }

    bool operator!=(const ImageInfo &other) const { return !(*this == other); }
};
} // namespace brayns
