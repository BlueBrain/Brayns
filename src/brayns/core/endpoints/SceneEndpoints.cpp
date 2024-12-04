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

#include "SceneEndpoints.h"

#include <brayns/core/objects/SceneObjects.h>

namespace brayns
{
void addSceneEndpoints(ApiBuilder &builder, ObjectManager &objects, Device &device)
{
    builder
        .endpoint("createGeometricModel", [&](CreateGeometricModelParams params) { return createGeometricModel(objects, device, std::move(params)); })
        .description("Create a geometric model from a geometry and material settings");
    builder.endpoint("getGeometricModel", [&](GetObjectParams params) { return getGeometricModel(objects, params); })
        .description("Get geometric model settings");

    builder.endpoint("createVolumetricModel", [&](CreateVolumetricModelParams params) { return createVolumetricModel(objects, device, params); })
        .description("Create a volumetric model from a volume and a transfer function");
    builder.endpoint("getVolumetricModel", [&](GetObjectParams params) { return getVolumetricModel(objects, params); })
        .description("Get volumetric model settings");

    builder.endpoint("createGroup", [&](CreateGroupParams params) { return createGroup(objects, device, params); })
        .description("Create a group of models and lights that share a common coordinate system and can be instanciated");
    builder.endpoint("getGroup", [&](GetObjectParams params) { return getGroup(objects, params); }).description("Get group settings");

    builder.endpoint("createInstance", [&](CreateInstanceParams params) { return createInstance(objects, device, params); })
        .description("Instanciate a group of objects, the resulting instance can be placed in a world");
    builder.endpoint("getInstance", [&](GetObjectParams params) { return getInstance(objects, params); }).description("Get instance settings");

    builder.endpoint("createWorld", [&](CreateWorldParams params) { return createWorld(objects, device, params); })
        .description("Create a world from a list of instances that can be rendered");
    builder.endpoint("getWorld", [&](GetObjectParams params) { return getWorld(objects, params); }).description("Get instance settings");
}
}
