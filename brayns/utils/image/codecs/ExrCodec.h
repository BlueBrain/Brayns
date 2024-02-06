/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include <optional>

namespace brayns
{
struct ExrFrame
{
    std::string name;
    Image image;
};

/**
 * @brief Encode and decode EXR images.
 */
class ExrCodec : public ImageCodec
{
public:
    /**
     * @brief Return the format ID of EXR ("exr").
     *
     * @return std::string EXR format ID.
     */
    std::string getFormat() const override;

    /**
     * @brief Encode an image as a single-frame Exr.
     *
     * @param image Image to encode.
     * @param quality Ignored for exr.
     * @param metadata Ignored for exr.
     * @return std::string EXR data that can be saved directly.
     */
    std::string encode(const Image &image, int quality, const std::optional<ImageMetadata> &metadata) const override;

    /**
     * @brief Encode a group of images as a multi-frame Exr.
     *
     * @param frames Images to encode into EXR.
     * @return std::string The encoded frames as EXR.
     */
    std::string encode(const std::vector<ExrFrame> &frames) const;

    /**
     * @brief Decode single-frame EXR data.
     *
     * @param data EXR data (ex: file content).
     * @param size Size of data in bytes.
     * @return Image Decoded image.
     */
    Image decode(const void *data, size_t size) const override;
};
} // namespace brayns
