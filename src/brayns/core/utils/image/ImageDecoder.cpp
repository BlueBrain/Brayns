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

#include "ImageDecoder.h"

#include <brayns/core/utils/FileReader.h>

#include "ImageCodecRegistry.h"
#include "ImageFormat.h"

namespace brayns
{
Image ImageDecoder::load(const std::string &filename)
{
    auto format = ImageFormat::fromFilename(filename);
    auto data = FileReader::read(filename);
    return decode(data, format);
}

Image ImageDecoder::decode(const std::string &data, const std::string &format)
{
    auto &codec = ImageCodecRegistry::getCodec(format);
    auto image = codec.decode(data.data(), data.size());
    if (image.isEmpty())
    {
        throw std::runtime_error("Failed to decode '" + format + "'");
    }
    return image;
}
} // namespace brayns
