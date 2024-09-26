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

#include "ImageOperationObjects.h"

namespace brayns
{
CreateObjectResult createToneMapper(ObjectManager &objects, Device &device, const CreateToneMapperParams &params)
{
    const auto &[base, derived] = params;

    auto operation = createToneMapper(device, derived);

    auto ptr = toShared(UserToneMapper{derived, std::move(operation)});

    auto object = UserImageOperation{
        .value = ptr,
        .get = [=] { return ptr->value; },
    };

    auto stored = objects.add(std::move(object), {"ToneMapper"}, base);

    return getResult(stored);
}

GetToneMapperResult getToneMapper(ObjectManager &objects, const GetObjectParams &params)
{
    auto object = objects.getAsStored<UserImageOperation>(params.id);
    auto &operation = *castAsShared<UserToneMapper>(object.get().value, object);
    return getResult(operation.settings);
}

void updateToneMapper(ObjectManager &objects, Device &device, const UpdateToneMapperParams &params)
{
    auto stored = objects.getAsStored<UserImageOperation>(params.id);
    auto &operation = *castAsShared<UserToneMapper>(stored.get().value, stored);

    auto settings = getUpdatedParams(params, operation.settings);

    operation.value.update(settings);
    device.throwIfError();

    operation.settings = settings;
}
}
