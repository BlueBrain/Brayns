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

#include "ImageOperationEndpoints.h"

#include <brayns/core/objects/ImageOperationObjects.h>

namespace brayns
{
ObjectResult createToneMapper(ObjectManager &manager, Device &device, const ToneMapperParams &params)
{
    return manager.visit([&](ObjectRegistry &objects) { return createToneMapper(objects, device, params); });
}

ToneMapperInfo getToneMapper(ObjectManager &manager, const ObjectParams &params)
{
    return manager.visit([&](ObjectRegistry &objects) { return getToneMapper(objects, params); });
}

void updateToneMapper(ObjectManager &manager, const ToneMapperUpdate &params)
{
    return manager.visit([&](ObjectRegistry &objects) { updateToneMapper(objects, params); });
}

void addImageOperationEndpoints(ApiBuilder &builder, ObjectManager &manager, Device &device)
{
    builder
        .endpoint("createToneMapper", [&](ToneMapperParams params) { return createToneMapper(manager, device, params); })
        .description("Create a tone mapper that can be attached to a framebuffer");

    builder.endpoint("getToneMapper", [&](ObjectParams params) { return getToneMapper(manager, params); })
        .description("Get tone mapper specific params");

    builder.endpoint("updateToneMapper", [&](ToneMapperUpdate params) { updateToneMapper(manager, params); })
        .description("Update tone mapper specific params");
}
}
