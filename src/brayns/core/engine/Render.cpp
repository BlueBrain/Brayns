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

#include "Render.h"

namespace brayns
{
bool Future::isReady() const
{
    auto handle = getHandle();
    return ospIsReady(handle);
}

float Future::getProgress() const
{
    auto handle = getHandle();
    return ospGetProgress(handle);
}

void Future::cancel()
{
    auto handle = getHandle();
    ospCancel(handle);
}

float Future::waitAndGetDuration()
{
    auto handle = getHandle();
    ospWait(handle);
    return ospGetTaskDuration(handle);
}

Future startRendering(OSPDevice device, const Context &context)
{
    auto framebuffer = context.framebuffer.getHandle();
    auto renderer = context.renderer.getHandle();
    auto camera = context.camera.getHandle();
    auto world = context.world.getHandle();

    auto handle = ospRenderFrame(framebuffer, renderer, camera, world);
    throwLastDeviceErrorIfNull(device, handle);

    return Future(handle);
}

std::optional<PickResult> tryPick(OSPDevice device, const PickSettings &settings)
{
    (void)device;

    auto framebuffer = settings.context.framebuffer.getHandle();
    auto renderer = settings.context.renderer.getHandle();
    auto camera = settings.context.camera.getHandle();
    auto world = settings.context.world.getHandle();
    auto [screenX, screenY] = settings.normalizedScreenPosition;

    auto result = OSPPickResult();
    ospPick(&result, framebuffer, renderer, camera, world, screenX, screenY);

    if (!result.hasHit)
    {
        return std::nullopt;
    }

    auto [x, y, z] = result.worldPosition;

    return PickResult{
        .worldPosition = {x, y, z},
        .instance = Instance(result.instance),
        .model = GeometricModel(result.model),
        .primitiveIndex = result.primID,
    };
}
}