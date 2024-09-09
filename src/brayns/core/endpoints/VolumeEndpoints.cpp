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

#include "VolumeEndpoints.h"

#include <brayns/core/objects/VolumeObjects.h>

namespace brayns
{
ObjectResult createRegularVolume(ObjectManager &manager, Device &device, const RegularVolumeParams &params)
{
    return manager.visit([&](ObjectRegistry &objects) { return createRegularVolume(objects, device, params); });
}

RegularVolumeInfo getRegularVolume(ObjectManager &manager, const ObjectParams &params)
{
    return manager.visit([&](ObjectRegistry &objects) { return getRegularVolume(objects, params); });
}

void updateRegularVolume(ObjectManager &manager, const RegularVolumeUpdate &params)
{
    return manager.visit([&](ObjectRegistry &objects) { updateRegularVolume(objects, params); });
}

void addVolumeEndpoints(ApiBuilder &builder, ObjectManager &manager, Device &device)
{
    builder
        .endpoint(
            "createRegularVolume",
            [&](RegularVolumeParams params) { return createRegularVolume(manager, device, params); })
        .description("Create a regular volume");

    builder.endpoint("getRegularVolume", [&](ObjectParams params) { return getRegularVolume(manager, params); })
        .description("Get regular volume specific params");

    builder.endpoint("updateRegularVolume", [&](RegularVolumeUpdate params) { updateRegularVolume(manager, params); })
        .description("Update regular volume specific params");
}
}
