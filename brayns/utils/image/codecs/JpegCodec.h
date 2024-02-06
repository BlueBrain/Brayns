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

#include <brayns/utils/image/ImageCodec.h>

namespace brayns
{
/**
 * @brief Encode and decode JPEG images.
 *
 */
class JpegCodec : public ImageCodec
{
public:
    /**
     * @brief Return the format ID of JPEG ("jpg").
     *
     * @return std::string JPEG format ID.
     */
    virtual std::string getFormat() const override;

    /**
     * @brief Encode an image as JPEG.
     *
     * @param image Image to encode.
     * @param quality Image quality (100 = better, 0 = smaller).
     * @param metadata Metadata to embed into the image.
     * @return std::string JPEG data that can be saved directly.
     */
    virtual std::string encode(
        const Image &image,
        int quality,
        const std::optional<ImageMetadata> &metadata) const override;

    /**
     * @brief Decode raw JPEG data.
     *
     * @param data JPEG data (ex: file content).
     * @param size Size of data in bytes.
     * @return Image Decoded image.
     */
    virtual Image decode(const void *data, size_t size) const override;
};
} // namespace brayns
