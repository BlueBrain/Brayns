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

#include "Image.h"

#include <cassert>
#include <cstring>
#include <stdexcept>

namespace brayns
{
Image::Image(const ImageInfo &info, std::string data):
    _info(info),
    _data(std::move(data))
{
    assert(_info.getSize() == _data.size());
}

Image::Image(const ImageInfo &info, char fillWith):
    _info(info),
    _data(info.getSize(), fillWith)
{
}

bool Image::isEmpty() const
{
    return _data.empty();
}

size_t Image::getWidth() const
{
    return _info.width;
}

size_t Image::getHeight() const
{
    return _info.height;
}

size_t Image::getChannelCount() const
{
    return _info.channelCount;
}

size_t Image::getChannelSize() const
{
    return _info.channelSize;
}

size_t Image::getSize() const
{
    return _data.size();
}

size_t Image::getRowSize() const
{
    return _info.getRowSize();
}

size_t Image::getPixelSize() const
{
    return _info.getPixelSize();
}

ImageDataType Image::getDataType() const
{
    return _info.dataType;
}

void Image::write(const Image &image, size_t x, size_t y)
{
    if (image.getChannelCount() != getChannelCount() || image.getChannelSize() != getChannelSize())
    {
        throw std::invalid_argument("Images have different shape");
    }

    if (image.getWidth() + x > getWidth() || image.getHeight() + y > getHeight())
    {
        throw std::out_of_range("Image write overflow");
    }

    auto rowSize = image.getRowSize();
    for (size_t i = 0; i < image.getHeight(); ++i)
    {
        auto src = static_cast<const char *>(image.getData()) + rowSize * i;
        write(src, rowSize, x, y + i);
    }
}

void Image::write(const void *data, size_t size, size_t x, size_t y)
{
    auto destination = getData(x, y);
    std::memcpy(destination, data, size);
}

const void *Image::getData(size_t x, size_t y) const
{
    auto offset = _info.getPixelOffset(x, y);
    return &_data[offset];
}

void *Image::getData(size_t x, size_t y)
{
    auto offset = _info.getPixelOffset(x, y);
    return &_data[offset];
}

bool Image::operator==(const Image &other) const
{
    return _info == other._info && _data == other._data;
}

bool Image::operator!=(const Image &other) const
{
    return !(*this == other);
}
} // namespace brayns
