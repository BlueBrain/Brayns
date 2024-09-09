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
ObjectResult createToneMapper(ObjectRegistry &objects, Device &device, const ToneMapperParams &params)
{
    auto operation = createToneMapper(device, params.derived);

    auto derived = UserToneMapper{params.derived, std::move(operation)};
    auto ptr = std::make_shared<decltype(derived)>(std::move(derived));

    auto object = UserImageOperation{
        .device = device,
        .value = ptr,
        .get = [=] { return ptr->value; },
    };

    auto stored = objects.add(std::move(object), "ToneMapper");

    return {stored.getId()};
}

ToneMapperInfo getToneMapper(ObjectRegistry &objects, const ObjectParams &params)
{
    auto stored = objects.getAsStored<UserImageOperation>(params.id);
    auto &operation = castAs<UserToneMapper>(stored.get().value, stored.getInfo());
    return operation.settings;
}

void updateToneMapper(ObjectRegistry &objects, const ToneMapperUpdate &params)
{
    auto stored = objects.getAsStored<UserImageOperation>(params.id);
    auto &base = stored.get();
    auto &derived = castAs<UserToneMapper>(base.value, stored.getInfo());
    auto &device = base.device.get();

    derived.value.update(params.settings);
    device.throwIfError();

    derived.settings = params.settings;
}
}
