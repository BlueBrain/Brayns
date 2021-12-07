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
class ImageEncoder
{
public:
    static void save(const Image &image, const std::string &filename,
                     int quality = 0)
    {
        auto format = ImageFormat::fromFilename(filename);
        auto data = encode(image, format, quality);
        FileWriter::write(data, filename);
    }

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
