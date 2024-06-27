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
using namespace brayns::experimental;

Data toSharedData2D(const void *data, TextureFormat format, const Size2 &size)
{
    auto type = getTextureDataType(format);
    auto handle = ospNewSharedData(data, type, size[0], 0, size[1]);
    return Data(handle);
}
}

namespace brayns::experimental
{
DataType getTextureDataType(TextureFormat format)
{
    switch (format)
    {
    case TextureFormat::Rgba8:
    case TextureFormat::Srgba8:
        return OSP_UINT;
    case TextureFormat::Rgba32F:
        return OSP_VEC4F;
    case TextureFormat::Rgb8:
    case TextureFormat::Srgb8:
        return OSP_VEC3UC;
    case TextureFormat::Rgb32F:
        return OSP_VEC3F;
    case TextureFormat::R8:
    case TextureFormat::L8:
        return OSP_UCHAR;
    case TextureFormat::Ra8:
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

OSPTexture ObjectReflector<Texture2D>::createHandle(OSPDevice device, const Settings &settings)
{
    auto handle = ospNewTexture("texture2D");
    throwLastDeviceErrorIfNull(device, handle);

    auto data = toSharedData2D(settings.data, settings.format, settings.size);

    setObjectParam(handle, "format", static_cast<OSPTextureFormat>(settings.format));
    setObjectParam(handle, "filter", static_cast<OSPTextureFilter>(settings.filter));
    setObjectParam(handle, "data", data);
    setObjectParam(handle, "wrapMode", static_cast<OSPTextureWrapMode>(settings.wrap));

    commitObject(handle);

    return handle;
}

OSPTexture ObjectReflector<VolumeTexture>::createHandle(OSPDevice device, const Settings &settings)
{
    auto handle = ospNewTexture("volume");
    throwLastDeviceErrorIfNull(device, handle);

    setObjectParam(handle, "volume", settings.volume);
    setObjectParam(handle, "transferFunction", settings.transferFunction);

    commitObject(handle);

    return handle;
}
}
