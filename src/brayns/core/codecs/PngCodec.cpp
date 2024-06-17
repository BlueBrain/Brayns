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

#include "PngCodec.h"

#include <memory>
#include <stdexcept>
#include <vector>

#include <fmt/format.h>

#include <png.h>

namespace
{
using namespace brayns::experimental;

std::uint32_t getImageFormat(ImageFormat format)
{
    switch (format)
    {
    case ImageFormat::Rgb8:
        return PNG_FORMAT_RGB;
    case ImageFormat::Rgba8:
        return PNG_FORMAT_RGBA;
    default:
        throw std::invalid_argument("Image format not supported by PNG encoder");
    }
}
}

namespace brayns::experimental
{
std::string encodePng(const ImageView &image)
{
    auto [data, size, format, rowOrder] = image;

    auto png = png_image{
        .opaque = nullptr,
        .version = PNG_IMAGE_VERSION,
        .width = static_cast<std::uint32_t>(size[0]),
        .height = static_cast<std::uint32_t>(size[1]),
        .format = getImageFormat(format),
        .flags = 0,
        .colormap_entries = 0,
        .warning_or_error = 0,
        .message = {},
    };

    auto rowStride = PNG_IMAGE_ROW_STRIDE(png);

    if (rowOrder == RowOrder::BottomUp)
    {
        rowStride = -rowStride;
    }

    auto convertTo8Bits = 0;
    auto colorMap = static_cast<const void *>(nullptr);

    auto bufferSize = static_cast<std::size_t>(PNG_IMAGE_BUFFER_SIZE(png, rowStride));
    auto buffer = std::string(bufferSize, '\0');

    auto code = png_image_write_to_memory(&png, buffer.data(), &bufferSize, convertTo8Bits, data, rowStride, colorMap);

    if (png.warning_or_error != 0)
    {
        auto message = static_cast<const char *>(png.message);
        throw std::runtime_error(fmt::format("PNG encoding failed: {}", message));
    }

    if (code == 0)
    {
        throw std::runtime_error("Unknown error during PNG encoding");
    }

    buffer.resize(bufferSize);

    return buffer;
}
}
