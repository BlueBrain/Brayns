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

#include "ImageEncoder.h"

#include <brayns/utils/FileWriter.h>

#include "ImageCodecRegistry.h"
#include "ImageFormat.h"

namespace brayns
{
void ImageEncoder::save(
    const Image &image,
    const std::string &filename,
    int quality,
    const std::optional<ImageMetadata> &metadata)
{
    auto format = ImageFormat::fromFilename(filename);
    auto data = encode(image, format, quality, metadata);
    FileWriter::write(data, filename);
}

std::string ImageEncoder::encode(
    const Image &image,
    const std::string &format,
    int quality,
    const std::optional<ImageMetadata> &metadata)
{
    auto &codec = ImageCodecRegistry::getCodec(format);
    auto data = codec.encode(image, quality, metadata);
    if (data.empty())
    {
        throw std::runtime_error("Failed to encode to '" + format + "'");
    }
    return data;
}
} // namespace brayns
