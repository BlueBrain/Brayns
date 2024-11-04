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

#pragma once

#include <any>
#include <concepts>
#include <functional>

#include <brayns/core/engine/Data.h>
#include <brayns/core/engine/TransferFunction.h>
#include <brayns/core/manager/ObjectManager.h>

namespace brayns
{
struct UserTransferFunction
{
    std::any value;
    std::function<TransferFunction()> get;
};

template<ReflectedJson Settings, std::derived_from<TransferFunction> T>
struct UserTransferFunctionOf
{
    Settings settings;
    T value;
};

template<>
struct JsonObjectReflector<LinearTransferFunctionSettings>
{
    static auto reflect()
    {
        auto builder = JsonBuilder<LinearTransferFunctionSettings>();
        builder.field("scalarRange", [](auto &object) { return &object.scalarRange; })
            .description("Range of the scalar values sampled from the volume that will be mapped to colors")
            .defaultValue(Box1(0, 1));
        builder.field("colors", [](auto &object) { return &object.colors; })
            .description("Colors to map the values sampled from the volume")
            .minItems(1)
            .defaultValue(std::vector<Color4>{Color4(0, 0, 0, 1), Color4(1, 1, 1, 1)});
        return builder.build();
    }
};

using CreateLinearTransferFunctionParams = CreateParamsOf<LinearTransferFunctionSettings>;
using GetLinearTransferFunctionResult = GetResultOf<LinearTransferFunctionSettings>;
using UpdateLinearTransferFunctionParams = UpdateParamsOf<LinearTransferFunctionSettings>;
using UserLinearTransferFunction = UserTransferFunctionOf<LinearTransferFunctionSettings, LinearTransferFunction>;

CreateObjectResult createLinearTransferFunction(ObjectManager &objects, Device &device, const CreateLinearTransferFunctionParams &params);
GetLinearTransferFunctionResult getLinearTransferFunction(ObjectManager &objects, const GetObjectParams &params);
void updateLinearTransferFunction(ObjectManager &objects, Device &device, const UpdateLinearTransferFunctionParams &params);
}
