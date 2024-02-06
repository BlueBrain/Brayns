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

#include <optional>
#include <string>
#include <vector>

#include "Image.h"
#include "ImageMetadata.h"

namespace brayns
{
/**
 * @brief Abstract encoder / decoder to encode and decode image files.
 *
 */
class ImageCodec
{
public:
    virtual ~ImageCodec() = default;

    /**
     * @brief Get the code of the image format (extension without dot).
     *
     * @return std::string Code of the supported format.
     */
    virtual std::string getFormat() const = 0;

    /**
     * @brief Encode the given image.
     *
     * Return an empty string if errors.
     *
     * @param image Image to encode.
     * @param quality Quality of the encoding if required (JPEG).
     * @param metadata Metadata to embed in the image.
     * @return std::string Encoded image data.
     */
    virtual std::string encode(const Image &image, int quality, const std::optional<ImageMetadata> &metadata) const = 0;

    /**
     * @brief Decode the given bytes.
     *
     * Return an empty image if errors.
     *
     * @param data Bytes to decode.
     * @param size Size of data.
     * @return Image Decoded image.
     */
    virtual Image decode(const void *data, size_t size) const = 0;
};
} // namespace brayns
