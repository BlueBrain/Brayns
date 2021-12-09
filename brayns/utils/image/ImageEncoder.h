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

#include <stdexcept>
#include <string>

#include <brayns/utils/FileWriter.h>
#include <brayns/utils/base64/base64.h>

#include "Image.h"
#include "ImageCodecRegistry.h"
#include "ImageFormat.h"

namespace brayns
{
/**
 * @brief Class to encode images to memory or file with base64 support.
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
     * @throw std::runtime_error Invalid format, path or image.
     */
    static void save(const Image &image, const std::string &filename,
                     int quality = 0)
    {
        auto format = ImageFormat::fromFilename(filename);
        auto data = encode(image, format, quality);
        FileWriter::write(data, filename);
    }

    /**
     * @brief Encode the image with given format.
     *
     * @param image Image to encode.
     * @param format Encoding format.
     * @param quality Image quality if compressed.
     * @return std::string Encoded image.
     * @throw std::runtime_error Invalid format or image.
     */
    static std::string encode(const Image &image, const std::string &format,
                              int quality = 0)
    {
        auto &codec = ImageCodecRegistry::getCodec(format);
        auto data = codec.encode(image, quality);
        if (data.empty())
        {
            throw std::runtime_error("Failed to encode to '" + format + "'");
        }
        return data;
    }

    /**
     * @brief Encode the image with given format and then to base64.
     *
     * @param image Image to encode.
     * @param format Encoding format.
     * @param quality Image quality if compressed.
     * @return std::string Encoded image in format and then base64.
     * @throw std::runtime_error Invalid format or image.
     */
    static std::string encodeToBase64(const Image &image,
                                      const std::string &format,
                                      int quality = 0)
    {
        auto data = encode(image, format, quality);
        auto bytes = reinterpret_cast<const unsigned char *>(data.data());
        auto size = data.size();
        return base64_encode(bytes, size);
    }
};
} // namespace brayns
