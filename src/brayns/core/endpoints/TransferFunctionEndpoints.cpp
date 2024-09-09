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

#include "TransferFunctionEndpoints.h"

#include <brayns/core/objects/TransferFunctionObjects.h>

namespace brayns
{
ObjectResult createLinearTransferFunction(
    ObjectManager &manager,
    Device &device,
    const LinearTransferFunctionParams &params)
{
    return manager.visit(
        [&](ObjectRegistry &objects) { return createLinearTransferFunction(objects, device, params); });
}

LinearTransferFunctionInfo getLinearTransferFunction(ObjectManager &manager, const ObjectParams &params)
{
    return manager.visit([&](ObjectRegistry &objects) { return getLinearTransferFunction(objects, params); });
}

void updateLinearTransferFunction(ObjectManager &manager, const LinearTransferFunctionUpdate &params)
{
    return manager.visit([&](ObjectRegistry &objects) { updateLinearTransferFunction(objects, params); });
}

void addTransferFunctionEndpoints(ApiBuilder &builder, ObjectManager &manager, Device &device)
{
    builder
        .endpoint(
            "createLinearTransferFunction",
            [&](LinearTransferFunctionParams params) { return createLinearTransferFunction(manager, device, params); })
        .description("Create a linear transfer function that can be attached to a volume");

    builder
        .endpoint(
            "getLinearTransferFunction",
            [&](ObjectParams params) { return getLinearTransferFunction(manager, params); })
        .description("Get linear transfer function specific params");

    builder
        .endpoint(
            "updateLinearTransferFunction",
            [&](LinearTransferFunctionUpdate params) { updateLinearTransferFunction(manager, params); })
        .description("Update linear transfer function specific params");
}
}
