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

#include "TransferFunctionObjects.h"

namespace
{
using namespace brayns;

LinearTransferFunctionSettings extractSettings(Device &device, const LinearTransferFunctionInfo &params)
{
    auto data = createData<Color4>(device, params.colors);
    return {params.scalarRange, std::move(data)};
}
}

namespace brayns
{
ObjectResult createLinearTransferFunction(
    ObjectRegistry &objects,
    Device &device,
    const LinearTransferFunctionParams &params)
{
    auto settings = extractSettings(device, params.derived);
    auto function = createLinearTransferFunction(device, settings);

    auto derived = UserLinearTransferFunction{params.derived, std::move(function)};
    auto ptr = std::make_shared<decltype(derived)>(std::move(derived));

    auto object = UserTransferFunction{
        .device = device,
        .value = ptr,
        .get = [=] { return ptr->value; },
    };

    auto stored = objects.add(std::move(object), "LinearTransferFunction");

    return {stored.getId()};
}

LinearTransferFunctionInfo getLinearTransferFunction(ObjectRegistry &objects, const ObjectParams &params)
{
    auto stored = objects.getAsStored<UserTransferFunction>(params.id);
    auto &operation = castAs<UserLinearTransferFunction>(stored.get().value, stored.getInfo());
    return operation.settings;
}

void updateLinearTransferFunction(ObjectRegistry &objects, const LinearTransferFunctionUpdate &params)
{
    auto stored = objects.getAsStored<UserTransferFunction>(params.id);
    auto &base = stored.get();
    auto &derived = castAs<UserLinearTransferFunction>(base.value, stored.getInfo());
    auto &device = base.device.get();

    auto settings = extractSettings(device, params.settings);

    derived.value.update(settings);
    device.throwIfError();

    derived.settings = params.settings;
}
}
