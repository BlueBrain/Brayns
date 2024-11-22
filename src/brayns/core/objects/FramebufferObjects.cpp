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

#include "FramebufferObjects.h"

#include "common/Objects.h"

namespace brayns
{
CreateObjectResult createFramebuffer(ObjectManager &objects, Device &device, const CreateFramebufferParams &params)
{
    const auto &[base, derived] = params;

    auto settings = derived.value;

    auto operations = getStoredObjects<UserImageOperation>(objects, derived.operations);
    settings.operations = getObjectHandles(operations);

    auto framebuffer = createFramebuffer(device, settings);

    auto object = UserFramebuffer{
        .settings = std::move(settings),
        .operations = std::move(operations),
        .value = std::move(framebuffer),
    };

    auto stored = objects.add(std::move(object), {"Framebuffer"}, base);

    return getResult(stored);
}

GetFramebufferResult getFramebuffer(ObjectManager &objects, const GetObjectParams &params)
{
    auto &framebuffer = objects.getAs<UserFramebuffer>(params.id);

    auto ids = getObjectIds(framebuffer.operations);
    auto variance = framebuffer.value.getVariance();

    return getResult(FramebufferInfo{{framebuffer.settings, std::move(ids)}, variance});
}

void clearFramebuffer(ObjectManager &objects, const GetObjectParams &params)
{
    auto &framebuffer = objects.getAs<UserFramebuffer>(params.id);
    framebuffer.value.resetAccumulation();
}
}
