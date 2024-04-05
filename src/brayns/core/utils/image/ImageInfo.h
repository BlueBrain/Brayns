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

#pragma once

#include <cstddef>

namespace brayns
{
enum class ImageDataType
{
    UnsignedInt,
    Float
};

/**
 * @brief Summary of all info about an image except its content.
 *
 */
struct ImageInfo
{
    /**
     * @brief Image width in pixels.
     *
     */
    size_t width = 0;

    /**
     * @brief Image height in pixels.
     *
     */
    size_t height = 0;

    /**
     * @brief Channel count.
     *
     * Usually:
     * 1 -> Grey scale
     * 2 -> Grey scale with alpha channel
     * 3 -> RGB
     * 4 -> RGBA
     *
     */
    size_t channelCount = 0;

    /**
     * @brief The size of a channel in bytes.
     *
     */
    size_t channelSize = 0;

    /**
     * @brief Underlying of the data stored in the image pixels.
     *
     */
    ImageDataType dataType = ImageDataType::UnsignedInt;

    /**
     * @brief Compute the size of the image in bytes.
     *
     * @return size_t Image size.
     */
    size_t getSize() const;

    /**
     * @brief Compute the size in bytes of a row.
     *
     * @return size_t Row size.
     */
    size_t getRowSize() const;

    /**
     * @brief Compute the number of pixels in the image.
     *
     * @return size_t Pixel count.
     */
    size_t getPixelCount() const;

    /**
     * @brief Compute the size of a pixel in bytes.
     *
     * @return size_t Pixel size.
     */
    size_t getPixelSize() const;

    /**
     * @brief Compute the index of a pixel in a contiguous array.
     *
     * @param x X offset.
     * @param y Y offset.
     * @return size_t Pixel index in a 1D array.
     */
    size_t getPixelIndex(size_t x, size_t y) const;

    /**
     * @brief Compute the offset in bytes of a pixel in a contiguous array.
     *
     * @param x X offset.
     * @param y Y offset.
     * @return size_t Pixel offset in a 1D array.
     */
    size_t getPixelOffset(size_t x, size_t y) const;

    /**
     * @brief Memberwise comparison.
     *
     * @param other Image info to compare with.
     * @return true Same image info.
     * @return false Different image info.
     */
    bool operator==(const ImageInfo &other) const;

    /**
     * @brief Memberwise comparison.
     *
     * @param other Image info to compare with.
     * @return true Different image info.
     * @return false Same image info.
     */
    bool operator!=(const ImageInfo &other) const;
};
} // namespace brayns
