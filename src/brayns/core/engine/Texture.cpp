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

namespace brayns
{
Texture2D createTexture2D(Device &device, const Texture2DSettings &settings)
{
    auto handle = ospNewTexture("texture2d");
    auto texture = wrapObjectHandleAs<Texture2D>(device, handle);

    setObjectParam(handle, "format", static_cast<OSPTextureFormat>(settings.format));
    setObjectParam(handle, "filter", static_cast<OSPTextureFilter>(settings.filter));
    setObjectParam(handle, "data", settings.data);
    setObjectParam(handle, "wrapMode", static_cast<OSPTextureWrapMode>(settings.wrap));

    commitObject(device, handle);

    return texture;
}

VolumeTexture createVolumeTexture(Device &device, const VolumeTextureSettings &settings)
{
    auto handle = ospNewTexture("volume");
    auto texture = wrapObjectHandleAs<VolumeTexture>(device, handle);

    setObjectParam(handle, "volume", settings.volume);
    setObjectParam(handle, "transferFunction", settings.transferFunction);

    commitObject(device, handle);

    return texture;
}
}
