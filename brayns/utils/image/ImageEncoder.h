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
#include <stdexcept>
#include <string>

#include "Image.h"
#include "ImageMetadata.h"

namespace brayns
{
/**
 * @brief Class to encode images to memory or file.
 *
 */
class ImageEncoder
{
public:
    /**
     * @brief Save image with given filename.
     *
     * @param image Image to save.
     * @param filename Path of the output file.
     * @param quality Image quality if compressed.
     * @param metadata Metadata to embed into the image.
     * @throw std::runtime_error Invalid format, path or image.
     */
    static void save(
        const Image &image,
        const std::string &filename,
        int quality = 0,
        const std::optional<ImageMetadata> &metadata = std::nullopt);

    /**
     * @brief Encode the image with given format.
     *
     * @param image Image to encode.
     * @param format Encoding format.
     * @param quality Image quality if compressed.
     * @param metadata Metadata to embed into the image.
     * @return std::string Encoded image.
     * @throw std::runtime_error Invalid format or image.
     */
    static std::string encode(
        const Image &image,
        const std::string &format,
        int quality = 0,
        const std::optional<ImageMetadata> &metadata = std::nullopt);
};
} // namespace brayns
