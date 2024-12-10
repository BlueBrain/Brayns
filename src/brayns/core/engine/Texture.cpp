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

#include "Texture.h"

namespace
{
using namespace brayns;

void setTexture2DParams(OSPTexture handle, const Texture2DSettings &settings)
{
    setObjectParam(handle, "filter", static_cast<OSPTextureFilter>(settings.filter));
    setObjectParam(handle, "wrapMode", static_cast<OSPTextureWrapMode>(settings.wrap));
}

void setVolumeTextureParams(OSPTexture handle, const VolumeTextureSettings &settings)
{
    setObjectParam(handle, "volume", settings.volume);
    setObjectParam(handle, "transferFunction", settings.transferFunction);
}
}

namespace brayns
{
DataType getDataType(TextureFormat format)
{
    switch (format)
    {
    case TextureFormat::Rgba8:
        return OSP_VEC4UC;
    case TextureFormat::Srgba8:
        return OSP_VEC4UC;
    case TextureFormat::Rgba32F:
        return OSP_VEC4F;
    case TextureFormat::Rgb8:
        return OSP_VEC3UC;
    case TextureFormat::Srgb8:
        return OSP_VEC3UC;
    case TextureFormat::Rgb32F:
        return OSP_VEC3F;
    case TextureFormat::R8:
        return OSP_UCHAR;
    case TextureFormat::Ra8:
        return OSP_VEC2UC;
    case TextureFormat::L8:
        return OSP_UCHAR;
    case TextureFormat::La8:
        return OSP_VEC2UC;
    case TextureFormat::R32F:
        return OSP_FLOAT;
    case TextureFormat::Rgba16:
        return OSP_VEC4US;
    case TextureFormat::Rgb16:
        return OSP_VEC3US;
    case TextureFormat::Ra16:
        return OSP_VEC2US;
    case TextureFormat::R16:
        return OSP_USHORT;
    default:
        throw std::invalid_argument("Invalid texture format");
    }
}

std::size_t getSize(TextureFormat format)
{
    switch (format)
    {
    case TextureFormat::Rgba8:
        return 4;
    case TextureFormat::Srgba8:
        return 4;
    case TextureFormat::Rgba32F:
        return 16;
    case TextureFormat::Rgb8:
        return 3;
    case TextureFormat::Srgb8:
        return 3;
    case TextureFormat::Rgb32F:
        return 12;
    case TextureFormat::R8:
        return 1;
    case TextureFormat::Ra8:
        return 2;
    case TextureFormat::L8:
        return 1;
    case TextureFormat::La8:
        return 2;
    case TextureFormat::R32F:
        return 4;
    case TextureFormat::Rgba16:
        return 8;
    case TextureFormat::Rgb16:
        return 6;
    case TextureFormat::Ra16:
        return 4;
    case TextureFormat::R16:
        return 2;
    default:
        throw std::invalid_argument("Invalid texture format");
    }
}

void Texture2D::update(const Texture2DSettings &settings)
{
    auto handle = getHandle();
    setTexture2DParams(handle, settings);
    commitObject(handle);
}

Texture2D createTexture2D(Device &device, const TextureData2D &data, const Texture2DSettings &settings)
{
    auto handle = ospNewTexture("texture2d");
    auto texture = wrapObjectHandleAs<Texture2D>(device, handle);

    auto type = getDataType(data.format);
    auto wrapper = createData(data.value.data(), type, Size3(data.size, 1));

    setObjectParam(handle, "data", wrapper);
    setObjectParam(handle, "format", static_cast<OSPTextureFormat>(data.format));

    setTexture2DParams(handle, settings);

    commitObject(device, handle);

    return texture;
}

void VolumeTexture::update(const VolumeTextureSettings &settings)
{
    auto handle = getHandle();
    setVolumeTextureParams(handle, settings);
    commitObject(handle);
}

VolumeTexture createVolumeTexture(Device &device, const VolumeTextureSettings &settings)
{
    auto handle = ospNewTexture("volume");
    auto texture = wrapObjectHandleAs<VolumeTexture>(device, handle);

    setVolumeTextureParams(handle, settings);

    commitObject(device, handle);

    return texture;
}
}
