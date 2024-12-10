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

#include "LightEndpoints.h"

#include <brayns/core/objects/LightObjects.h>

namespace brayns
{
void addLightEndpoints(ApiBuilder &builder, ObjectManager &objects, Device &device)
{
    builder.endpoint("getLight", [&](GetObjectParams params) { return getLight(objects, params); }).description("Get a light of any type");
    builder.endpoint("updateLight", [&](UpdateLightParams params) { updateLight(objects, device, params); }).description("Update a light of any type");

    builder.endpoint("createDistantLight", [&](CreateDistantLightParams params) { return createDistantLight(objects, device, params); })
        .description("Create a new distant light");
    builder.endpoint("getDistantLight", [&](GetObjectParams params) { return getDistantLight(objects, params); })
        .description("Get distant light specific params");
    builder.endpoint("updateDistantLight", [&](UpdateDistantLightParams params) { updateDistantLight(objects, device, params); })
        .description("Update distant light specific params");

    builder.endpoint("createSphereLight", [&](CreateSphereLightParams params) { return createSphereLight(objects, device, params); })
        .description("Create a new sphere light");
    builder.endpoint("getSphereLight", [&](GetObjectParams params) { return getSphereLight(objects, params); })
        .description("Get sphere light specific params");
    builder.endpoint("updateSphereLight", [&](UpdateSphereLightParams params) { updateSphereLight(objects, device, params); })
        .description("Update sphere light specific params");

    builder.endpoint("createSpotLight", [&](CreateSpotLightParams params) { return createSpotLight(objects, device, params); })
        .description("Create a new spot light");
    builder.endpoint("getSpotLight", [&](GetObjectParams params) { return getSpotLight(objects, params); })
        .description("Get spot light specific params");
    builder.endpoint("updateSpotLight", [&](UpdateSpotLightParams params) { updateSpotLight(objects, device, params); })
        .description("Update spot light specific params");

    builder.endpoint("createQuadLight", [&](CreateQuadLightParams params) { return createQuadLight(objects, device, params); })
        .description("Create a new quad light");
    builder.endpoint("getQuadLight", [&](GetObjectParams params) { return getQuadLight(objects, params); })
        .description("Get quad light specific params");
    builder.endpoint("updateQuadLight", [&](UpdateQuadLightParams params) { updateQuadLight(objects, device, params); })
        .description("Update quad light specific params");

    builder.endpoint("createCylinderLight", [&](CreateCylinderLightParams params) { return createCylinderLight(objects, device, params); })
        .description("Create a new cylinder light");
    builder.endpoint("getCylinderLight", [&](GetObjectParams params) { return getCylinderLight(objects, params); })
        .description("Get cylinder light specific params");
    builder.endpoint("updateCylinderLight", [&](UpdateCylinderLightParams params) { updateCylinderLight(objects, device, params); })
        .description("Update cylinder light specific params");

    builder.endpoint("createHdriLight", [&](CreateHdriLightParams params) { return createHdriLight(objects, device, params); })
        .description("Create a new HDRI light");
    builder.endpoint("getHdriLight", [&](GetObjectParams params) { return getHdriLight(objects, params); })
        .description("Get HDRI light specific params");
    builder.endpoint("updateHdriLight", [&](UpdateHdriLightParams params) { updateHdriLight(objects, device, params); })
        .description("Update HDRI light specific params");

    builder.endpoint("createAmbientLight", [&](CreateAmbientLightParams params) { return createAmbientLight(objects, device, params); })
        .description("Create a new ambient light");
    builder.endpoint("getAmbientLight", [&](GetObjectParams params) { return getAmbientLight(objects, params); })
        .description("Get ambient light specific params");
    builder.endpoint("updateAmbientLight", [&](UpdateAmbientLightParams params) { updateAmbientLight(objects, device, params); })
        .description("Update ambient light specific params");

    builder.endpoint("createSunSkyLight", [&](CreateSunSkyLightParams params) { return createSunSkyLight(objects, device, params); })
        .description("Create a new sun sky light");
    builder.endpoint("getSunSkyLight", [&](GetObjectParams params) { return getSunSkyLight(objects, params); })
        .description("Get sun sky light specific params");
    builder.endpoint("updateSunSkyLight", [&](UpdateSunSkyLightParams params) { updateSunSkyLight(objects, device, params); })
        .description("Update sun sky light specific params");
}
}
