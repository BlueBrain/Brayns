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
#include <string>

#include "ImageInfo.h"

namespace brayns
{
class Image
{
public:
    Image() = default;

    Image(const ImageInfo &info, std::string data)
        : _info(info)
        , _data(std::move(data))
    {
        assert(_info.getSize() == _data.size());
    }

    Image(const ImageInfo &info, char fillWith = '\0')
        : _info(info)
        , _data(info.getSize(), fillWith)
    {
    }

    bool isEmpty() const { return getData() == nullptr; }

    const ImageInfo &getInfo() const { return _info; }

    size_t getWidth() const { return _info.width; }

    size_t getHeight() const { return _info.height; }

    size_t getChannelCount() const { return _info.channelCount; }

    size_t getChannelSize() const { return _info.channelSize; }

    size_t getSize() const { return _data.size(); }

    size_t getRowSize() const { return _info.getRowSize(); }

    size_t getPixelSize() const { return _info.getPixelSize(); }

    void write(const Image &image, size_t x = 0, size_t y = 0)
    {
        write(image.getData(), image.getSize(), x, y);
    }

    void write(const void *data, size_t size, size_t x = 0, size_t y = 0)
    {
        auto destination = getData(x, y);
        std::memcpy(destination, data, size);
    }

    const void *getData(size_t x = 0, size_t y = 0) const
    {
        auto offset = _info.getPixelOffset(x, y);
        return &_data[offset];
    }

    void *getData(size_t x = 0, size_t y = 0)
    {
        auto offset = _info.getPixelOffset(x, y);
        return &_data[offset];
    }

private:
    ImageInfo _info;
    std::string _data;
};
} // namespace brayns
