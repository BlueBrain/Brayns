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

#include <brayns/core/jsonrpc/Errors.h>
#include <brayns/core/objects/CameraObjects.h>

namespace
{
using namespace brayns;

void validateCameraSettings(const CameraSettings &settings)
{
    auto &view = settings.view;
    auto right = cross(view.direction, view.up);

    if (right.x == 0.0F && right.y == 0.0F && right.z == 0.0F)
    {
        throw InvalidParams("Camera up and direction are colinear");
    }
}
}

namespace brayns
{
CameraInfo getCamera(ObjectManager &manager, const ObjectParams &params)
{
    return manager.visit([&](ObjectRegistry &objects) { return getCamera(objects, params); });
}

void updateCamera(ObjectManager &manager, const CameraUpdate &params)
{
    validateCameraSettings(params.settings);
    manager.visit([&](ObjectRegistry &objects) { updateCamera(objects, params); });
}

ObjectResult createPerspectiveCamera(ObjectManager &manager, Device &device, const PerspectiveCameraParams &params)
{
    validateCameraSettings(params.base);
    return manager.visit([&](ObjectRegistry &objects) { return createPerspectiveCamera(objects, device, params); });
}

PerspectiveCameraInfo getPerspectiveCamera(ObjectManager &manager, const ObjectParams &params)
{
    return manager.visit([&](ObjectRegistry &objects) { return getPerspectiveCamera(objects, params); });
}

void updatePerspectiveCamera(ObjectManager &manager, const PerspectiveCameraUpdate &params)
{
    manager.visit([&](ObjectRegistry &objects) { return updatePerspectiveCamera(objects, params); });
}

ObjectResult createOrthographicCamera(ObjectManager &manager, Device &device, const OrthographicCameraParams &params)
{
    validateCameraSettings(params.base);
    return manager.visit([&](ObjectRegistry &objects) { return createOrthographicCamera(objects, device, params); });
}

OrthographicCameraInfo getOrthographicCamera(ObjectManager &manager, const ObjectParams &params)
{
    return manager.visit([&](ObjectRegistry &objects) { return getOrthographicCamera(objects, params); });
}

void updateOrthographicCamera(ObjectManager &manager, const OrthographicCameraUpdate &params)
{
    manager.visit([&](ObjectRegistry &objects) { return updateOrthographicCamera(objects, params); });
}

ObjectResult createPanoramicCamera(ObjectManager &manager, Device &device, const PanoramicCameraParams &params)
{
    validateCameraSettings(params.base);
    return manager.visit([&](ObjectRegistry &objects) { return createPanoramicCamera(objects, device, params); });
}

PanoramicCameraInfo getPanoramicCamera(ObjectManager &manager, const ObjectParams &params)
{
    return manager.visit([&](ObjectRegistry &objects) { return getPanoramicCamera(objects, params); });
}

void updatePanoramicCamera(ObjectManager &manager, const PanoramicCameraUpdate &params)
{
    manager.visit([&](ObjectRegistry &objects) { return updatePanoramicCamera(objects, params); });
}

void addCameraEndpoints(ApiBuilder &builder, ObjectManager &manager, Device &device)
{
    builder.endpoint("getCamera", [&](ObjectParams params) { return getCamera(manager, params); })
        .description("Get a camera of any type");
    builder.endpoint("updateCamera", [&](CameraUpdate params) { updateCamera(manager, params); })
        .description("Update a camera of any type");

    builder
        .endpoint(
            "createPerspectiveCamera",
            [&](PerspectiveCameraParams params) { return createPerspectiveCamera(manager, device, params); })
        .description("Create a new perspective camera");
    builder.endpoint("getPerspectiveCamera", [&](ObjectParams params) { return getPerspectiveCamera(manager, params); })
        .description("Get perspective camera specific params");
    builder
        .endpoint(
            "updatePerspectiveCamera",
            [&](PerspectiveCameraUpdate params) { updatePerspectiveCamera(manager, params); })
        .description("Update perspective camera specific params");

    builder
        .endpoint(
            "createOrthographicCamera",
            [&](OrthographicCameraParams params) { return createOrthographicCamera(manager, device, params); })
        .description("Create a new panoramic camera");
    builder
        .endpoint("getOrthographicCamera", [&](ObjectParams params) { return getOrthographicCamera(manager, params); })
        .description("Get panoramic camera specific params");
    builder
        .endpoint(
            "updateOrthographicCamera",
            [&](OrthographicCameraUpdate params) { updateOrthographicCamera(manager, params); })
        .description("Update panoramic camera specific params");

    builder
        .endpoint(
            "createPanoramicCamera",
            [&](PanoramicCameraParams params) { return createPanoramicCamera(manager, device, params); })
        .description("Create a new panoramic camera");
    builder.endpoint("getPanoramicCamera", [&](ObjectParams params) { return getPanoramicCamera(manager, params); })
        .description("Get panoramic camera specific params");
    builder
        .endpoint("updatePanoramicCamera", [&](PanoramicCameraUpdate params) { updatePanoramicCamera(manager, params); })
        .description("Update panoramic camera specific params");
}
}
