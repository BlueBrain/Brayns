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

#include "MaterialEndpoints.h"

#include <brayns/core/objects/MaterialObjects.h>

namespace brayns
{
void addMaterialEndpoints(ApiBuilder &builder, ObjectManager &objects, Device &device)
{
    builder.endpoint("createAoMaterial", [&](CreateAoMaterialParams params) { return createAoMaterial(objects, device, params); })
        .description("Create a material than can be used by the ambient occlusion renderer");
    builder.endpoint("getAoMaterial", [&](GetObjectParams params) { return getAoMaterial(objects, params); }).description("Get AO material");
    builder.endpoint("updateAoMaterial", [&](UpdateAoMaterialParams params) { return updateAoMaterial(objects, device, params); })
        .description("Update AO material");

    builder.endpoint("createScivisMaterial", [&](CreateScivisMaterialParams params) { return createScivisMaterial(objects, device, params); })
        .description("Create a material than can be used by the scivis renderer");
    builder.endpoint("getScivisMaterial", [&](GetObjectParams params) { return getScivisMaterial(objects, params); })
        .description("Get scivis material");
    builder.endpoint("updateScivisMaterial", [&](UpdateScivisMaterialParams params) { return updateScivisMaterial(objects, device, params); })
        .description("Update scivis material");

    builder
        .endpoint("createPrincipledMaterial", [&](CreatePrincipledMaterialParams params) { return createPrincipledMaterial(objects, device, params); })
        .description("Create a material than can be used by the path tracer");
    builder.endpoint("getPrincipledMaterial", [&](GetObjectParams params) { return getPrincipledMaterial(objects, params); })
        .description("Get principled material");
    builder
        .endpoint("updatePrincipledMaterial", [&](UpdatePrincipledMaterialParams params) { return updatePrincipledMaterial(objects, device, params); })
        .description("Update principled material");
}
}
