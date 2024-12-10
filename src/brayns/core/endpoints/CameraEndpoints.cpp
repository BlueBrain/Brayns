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

#include "CameraEndpoints.h"

#include <brayns/core/objects/CameraObjects.h>

namespace brayns
{
void addCameraEndpoints(ApiBuilder &builder, ObjectManager &objects, Device &device)
{
    builder.endpoint("getCamera", [&](GetObjectParams params) { return getCamera(objects, params); }).description("Get a camera of any type");
    builder.endpoint("updateCamera", [&](UpdateCameraParams params) { updateCamera(objects, device, params); })
        .description("Update a camera of any type");

    builder
        .endpoint("createPerspectiveCamera", [&](CreatePerspectiveCameraParams params) { return createPerspectiveCamera(objects, device, params); })
        .description("Create a new perspective camera");
    builder.endpoint("getPerspectiveCamera", [&](GetObjectParams params) { return getPerspectiveCamera(objects, params); })
        .description("Get perspective camera specific params");
    builder.endpoint("updatePerspectiveCamera", [&](UpdatePerspectiveCameraParams params) { updatePerspectiveCamera(objects, device, params); })
        .description("Update perspective camera specific params");

    builder
        .endpoint("createOrthographicCamera", [&](CreateOrthographicCameraParams params) { return createOrthographicCamera(objects, device, params); })
        .description("Create a new panoramic camera");
    builder.endpoint("getOrthographicCamera", [&](GetObjectParams params) { return getOrthographicCamera(objects, params); })
        .description("Get panoramic camera specific params");
    builder.endpoint("updateOrthographicCamera", [&](UpdateOrthographicCameraParams params) { updateOrthographicCamera(objects, device, params); })
        .description("Update panoramic camera specific params");

    builder.endpoint("createPanoramicCamera", [&](CreatePanoramicCameraParams params) { return createPanoramicCamera(objects, device, params); })
        .description("Create a new panoramic camera");
    builder.endpoint("getPanoramicCamera", [&](GetObjectParams params) { return getPanoramicCamera(objects, params); })
        .description("Get panoramic camera specific params");
    builder.endpoint("updatePanoramicCamera", [&](UpdatePanoramicCameraParams params) { updatePanoramicCamera(objects, device, params); })
        .description("Update panoramic camera specific params");
}
}
