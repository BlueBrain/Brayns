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

#include "TextureEndpoints.h"

#include <brayns/core/objects/TextureObjects.h>

namespace brayns
{
void addTextureEndpoints(ApiBuilder &builder, ObjectManager &objects, Device &device)
{
    builder.endpoint("createTexture2D", [&](CreateTexture2DParams params) { return createTexture2D(objects, device, std::move(params)); })
        .description("Create a 2D texture");
    builder.endpoint("getTexture2D", [&](GetObjectParams params) { return getTexture2D(objects, params); })
        .description("Get 2D texture specific params");
    builder.endpoint("updateTexture2D", [&](UpdateTexture2DParams params) { updateTexture2D(objects, device, params); })
        .description("Update 2D texture specific params");

    builder.endpoint("createVolumeTexture", [&](CreateVolumeTextureParams params) { return createVolumeTexture(objects, device, params); })
        .description("Create a volume texture");
    builder.endpoint("getVolumeTexture", [&](GetObjectParams params) { return getVolumeTexture(objects, params); })
        .description("Get volume texture specific params");
    builder.endpoint("updateVolumeTexture", [&](UpdateVolumeTextureParams params) { updateVolumeTexture(objects, device, params); })
        .description("Update volume texture specific params");
}
}
