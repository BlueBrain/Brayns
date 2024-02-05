/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#include <brayns/utils/image/Image.h>

namespace brayns
{
/**
 * @brief Helper class to build and use STB image.
 *
 * Only supports 8bit channels images.
 */
class StbiHelper
{
public:
    /**
     * @brief Decode byte array of encoded image and return the image.
     *
     * The byte array is the raw encoded image (ex: PNG file content).
     *
     * The format is deduced from the data directly.
     *
     * @param data Raw encoded image data.
     * @param size Size of data in bytes.
     * @return Image Decoded image.
     */
    static Image decode(const void *data, size_t size);

    /**
     * @brief Encode an image as PNG.
     *
     * The resulting PNG data can be written directly to the file.
     *
     * @param image Image to encode.
     * @return std::string Image PNG data.
     */
    static std::string encodePng(const Image &image);

    /**
     * @brief Encode an image as JPEG.
     *
     * The resulting JPEG data can be written directly to the file.
     *
     * Alpha channel is discared.
     *
     * @param image Image to encode.
     * @return std::string Image JPEG data.
     */
    static std::string encodeJpeg(const Image &image, int quality);
};
} // namespace brayns
