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

#include "TransferFunction.h"

namespace
{
using namespace brayns;

void setLinearTransferFunctionParams(OSPTransferFunction handle, const LinearTransferFunctionSettings &settings)
{
    setObjectParam(handle, "value", settings.scalarRange);

    auto data = reinterpret_cast<const char *>(settings.colors.data());
    auto itemCount = Size3(settings.colors.size(), 1, 1);
    auto stride = Stride3(static_cast<std::ptrdiff_t>(sizeof(Color4)), 0, 0);
    auto opacityOffset = sizeof(Color3);

    auto colors = createData(data, OSP_VEC3F, itemCount, stride);
    setObjectParam(handle, "color", colors);

    auto opacities = createData(data + opacityOffset, OSP_FLOAT, itemCount, stride);
    setObjectParam(handle, "opacity", opacities);
}
}

namespace brayns
{
void LinearTransferFunction::update(const LinearTransferFunctionSettings &settings)
{
    auto handle = getHandle();
    setLinearTransferFunctionParams(handle, settings);
    commitObject(handle);
}

LinearTransferFunction createLinearTransferFunction(Device &device, const LinearTransferFunctionSettings &settings)
{
    auto handle = ospNewTransferFunction("piecewiseLinear");
    auto transferFunction = wrapObjectHandleAs<LinearTransferFunction>(device, handle);

    setLinearTransferFunctionParams(handle, settings);

    commitObject(device, handle);

    return transferFunction;
}
}
