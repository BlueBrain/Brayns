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

#include "JpegCodec.h"

#include <memory>
#include <stdexcept>

#include <fmt/format.h>

#include <turbojpeg.h>

namespace
{
using namespace brayns::experimental;

struct Deleter
{
    void operator()(tjhandle handle) const
    {
        tjDestroy(handle);
    }
};

using Holder = std::unique_ptr<void, Deleter>;

TJPF getPixelFormat(ImageFormat format)
{
    switch (format)
    {
    case ImageFormat::Rgb:
        return TJPF_RGB;
    case ImageFormat::Rgba:
        return TJPF_RGBA;
    default:
        throw std::runtime_error(fmt::format("Invalid pixel format for JPEG encoder: {}", static_cast<int>(format)));
    }
}

std::runtime_error lastError(tjhandle handle, const char *message)
{
    const auto *description = tj3GetErrorStr(handle);
    return std::runtime_error(fmt::format("{}: {}", message, description));
}

void setParam(tjhandle handle, TJPARAM param, int value)
{
    auto result = tj3Set(handle, param, value);

    if (result != 0)
    {
        throw lastError(handle, "Failed to set JPEG param");
    }
}
}

namespace brayns::experimental
{
std::string encodeJpeg(const ImageView &image, int quality)
{
    auto *compressor = tjInitCompress();

    if (compressor == nullptr)
    {
        throw std::runtime_error("Failed to allocate JPEG encoder");
    }

    auto holder = Holder(compressor);

    auto &[data, size, format] = image;

    const auto *source = static_cast<const unsigned char *>(data);
    auto width = static_cast<int>(size[0]);
    auto height = static_cast<int>(size[1]);
    auto pitch = 0;
    auto pixelFormat = static_cast<int>(getPixelFormat(format));
    auto subsampling = TJSAMP_444;

    setParam(compressor, TJPARAM_STOPONWARNING, 1);
    setParam(compressor, TJPARAM_BOTTOMUP, 1);
    setParam(compressor, TJPARAM_NOREALLOC, 1);
    setParam(compressor, TJPARAM_QUALITY, quality);
    setParam(compressor, TJPARAM_SUBSAMP, subsampling);

    auto bufferSize = tj3JPEGBufSize(width, height, subsampling);

    auto buffer = std::string(bufferSize, '\0');
    auto *bufferPtr = reinterpret_cast<unsigned char *>(buffer.data());

    auto **output = &bufferPtr;

    auto result = tj3Compress8(compressor, source, width, pitch, height, pixelFormat, output, &bufferSize);

    if (result == 0)
    {
        buffer.resize(bufferSize);
        return buffer;
    }

    const auto *lastError = tj3GetErrorStr(compressor);
    throw std::runtime_error(fmt::format("JPEG encoding failed: {}", lastError));
}
}
