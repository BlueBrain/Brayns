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

namespace brayns
{
CreateObjectResult createLinearTransferFunction(ObjectManager &objects, Device &device, const CreateLinearTransferFunctionParams &params)
{
    const auto &[base, derived] = params;

    auto operation = createLinearTransferFunction(device, derived);

    auto ptr = toShared(UserLinearTransferFunction{std::move(derived), std::move(operation)});

    auto object = UserTransferFunction{
        .value = ptr,
        .get = [=] { return ptr->value; },
    };

    auto stored = objects.add(std::move(object), {"LinearTransferFunction"}, base);

    return getResult(stored);
}

GetLinearTransferFunctionResult getLinearTransferFunction(ObjectManager &objects, const GetObjectParams &params)
{
    auto object = objects.getAsStored<UserTransferFunction>(params.id);
    auto &operation = *castAsShared<UserLinearTransferFunction>(object.get().value, object);
    return getResult(operation.settings);
}

void updateLinearTransferFunction(ObjectManager &objects, Device &device, const UpdateLinearTransferFunctionParams &params)
{
    auto stored = objects.getAsStored<UserTransferFunction>(params.id);
    auto &operation = *castAsShared<UserLinearTransferFunction>(stored.get().value, stored);

    auto settings = getUpdatedParams(params, operation.settings);

    operation.value.update(settings);
    device.throwIfError();

    operation.settings = std::move(settings);
}
}
