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
#include <string>

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

    size_t getPixelCount() const { return width * height; }

    size_t getPixelSize() const { return channelCount * channelSize; }

    bool isGrey() const { return channelCount == 1; }

    bool isGreyAlpha() const { return channelCount == 2; }

    bool isRgb() const { return channelCount == 3; }

    bool isRgba() const { return channelCount == 4; }
};

class Image
{
public:
    static Image load(const std::string &filename);

    Image() = default;
    Image(const ImageInfo &info);
    Image(const ImageInfo &info, void *data);
    ~Image();
    Image(const Image &other);
    Image(Image &&other);
    Image &operator=(const Image &other);
    Image &operator=(Image &&other);

    void save(const std::string &filename) const;
    void flipVertically();

    const ImageInfo &getInfo() const { return _info; }

    size_t getWidth() const { return _info.width; }

    size_t getHeight() const { return _info.height; }

    size_t getChannelCount() const { return _info.channelCount; }

    size_t getChannelSize() const { return _info.channelSize; }

    size_t getSize() const { return _info.getSize(); }

    size_t getRowSize() const { return _info.getRowSize(); }

    size_t getPixelCount() const { return _info.getPixelCount(); }

    size_t getPixelSize() const { return _info.getPixelSize(); }

    bool isEmpty() const { return _data == nullptr; }

    void *getData() const { return _data; }

    uint8_t *getBytes() const { return getDataAs<uint8_t>(); }

    uint16_t *getShorts() const { return getDataAs<uint16_t>(); }

    float *getFloats() const { return getDataAs<float>(); }

    template <typename T>
    T *getDataAs() const
    {
        assert(_info.channelSize == sizeof(T));
        return static_cast<T *>(_data);
    }

private:
    ImageInfo _info;
    void *_data = nullptr;
};
} // namespace brayns
