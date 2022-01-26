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
/**
 * @brief Generic image representation.
 *
 * Data are stored as a contiguous memory block with channels RGBA in this
 * order (if present).
 *
 * First pixel is top-left, last one is bottom-right.
 *
 * Images can be moved and copied like normal objects (no data sharing).
 *
 */
class Image
{
public:
    /**
     * @brief Construct an empty image (no data and size of zero).
     *
     */
    Image() = default;

    /**
     * @brief Construct an image using its metadata and content.
     *
     * Low level API, info.getSize() != data.size() is undefined behavior.
     *
     * @param info Image info.
     * @param data Image content with contiguous pixels RGBA.
     */
    Image(const ImageInfo &info, std::string data)
        : _info(info)
        , _data(std::move(data))
    {
        assert(_info.getSize() == _data.size());
    }

    /**
     * @brief Construct a new image filled with the given character.
     *
     * @param info Image info.
     * @param fillWith Fill image with this byte.
     */
    Image(const ImageInfo &info, char fillWith = '\0')
        : _info(info)
        , _data(info.getSize(), fillWith)
    {
    }

    /**
     * @brief Check wether the image is empty (width = height = 0).
     *
     * @return true Empty image.
     * @return false Valid image.
     */
    bool isEmpty() const
    {
        return getData() == nullptr;
    }

    /**
     * @brief Get the image info.
     *
     * @return const ImageInfo& Image info.
     */
    const ImageInfo &getInfo() const
    {
        return _info;
    }

    /**
     * @brief Get the image width in pixels.
     *
     * @return size_t Image width.
     */
    size_t getWidth() const
    {
        return _info.width;
    }

    /**
     * @brief Get the image height in pixels.
     *
     * @return size_t Image height.
     */
    size_t getHeight() const
    {
        return _info.height;
    }

    /**
     * @brief Get the number of channels per pixel.
     *
     * 4 -> RGBA
     * 3 -> RGB
     * 2 -> grey alpha
     * 1 -> grey
     *
     * @return size_t Channels per pixel.
     */
    size_t getChannelCount() const
    {
        return _info.channelCount;
    }

    /**
     * @brief Get the size of a channel in bytes.
     *
     * @return size_t Channel size.
     */
    size_t getChannelSize() const
    {
        return _info.channelSize;
    }

    /**
     * @brief Get the size of the image in bytes.
     *
     * @return size_t Image size.
     */
    size_t getSize() const
    {
        return _data.size();
    }

    /**
     * @brief Get the size of a pixel row in bytes.
     *
     * @return size_t Row size.
     */
    size_t getRowSize() const
    {
        return _info.getRowSize();
    }

    /**
     * @brief Get the size of a single pixel in bytes.
     *
     * @return size_t Pixel size.
     */
    size_t getPixelSize() const
    {
        return _info.getPixelSize();
    }

    /**
     * @brief Write the given image inside the current one with given offset.
     *
     * X and Y will be the top-left position of the image in this one.
     *
     * @param image Image to copy.
     * @param x X offset.
     * @param y Y offset.
     */
    void write(const Image &image, size_t x = 0, size_t y = 0)
    {
        write(image.getData(), image.getSize(), x, y);
    }

    /**
     * @brief Write the given data with the given offset.
     *
     * The data will be copied from XY to XY + size.
     *
     * @param data Data to copy in this image.
     * @param size Size of data.
     * @param x X offset.
     * @param y Y offset.
     */
    void write(const void *data, size_t size, size_t x = 0, size_t y = 0)
    {
        auto destination = getData(x, y);
        std::memcpy(destination, data, size);
    }

    /**
     * @brief Get the immutable raw image data with given offset.
     *
     * @param x X offset.
     * @param y Y offset.
     * @return const void* Image data.
     */
    const void *getData(size_t x = 0, size_t y = 0) const
    {
        auto offset = _info.getPixelOffset(x, y);
        return &_data[offset];
    }

    /**
     * @brief Get the mutable raw image data with given offset.
     *
     * @param x X offset.
     * @param y Y offset.
     * @return const void* Image data.
     */
    void *getData(size_t x = 0, size_t y = 0)
    {
        auto offset = _info.getPixelOffset(x, y);
        return &_data[offset];
    }

    /**
     * @brief Compare two images pixel by pixel.
     *
     * @param other Image to compare.
     * @return true Same info and data.
     * @return false Different info or data.
     */
    bool operator==(const Image &other) const
    {
        return _info == other._info && _data == other._data;
    }

    /**
     * @brief Compare two images pixel by pixel.
     *
     * @param other Image to compare.
     * @return true Different info or data.
     * @return false Same info and data.
     */
    bool operator!=(const Image &other) const
    {
        return !(*this == other);
    }

private:
    ImageInfo _info;
    std::string _data;
};
} // namespace brayns
