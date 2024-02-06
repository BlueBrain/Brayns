/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
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

#include "ImageInfo.h"

#include <stdexcept>

namespace brayns
{
size_t ImageInfo::getSize() const
{
    return getPixelCount() * getPixelSize();
}

size_t ImageInfo::getRowSize() const
{
    return width * getPixelSize();
}

size_t ImageInfo::getPixelCount() const
{
    return width * height;
}

size_t ImageInfo::getPixelSize() const
{
    return channelCount * channelSize;
}

size_t ImageInfo::getPixelIndex(size_t x, size_t y) const
{
    if (x >= width || y >= height)
    {
        throw std::out_of_range("Image coordinates out of bounds");
    }

    return x + y * width;
}

size_t ImageInfo::getPixelOffset(size_t x, size_t y) const
{
    return getPixelIndex(x, y) * getPixelSize();
}

bool ImageInfo::operator==(const ImageInfo &other) const
{
    return width == other.width && height == other.height && channelCount == other.channelCount
        && channelSize == other.channelSize;
}

bool ImageInfo::operator!=(const ImageInfo &other) const
{
    return !(*this == other);
}
} // namespace brayns
