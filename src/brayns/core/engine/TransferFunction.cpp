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

namespace brayns
{
LinearTransferFunction createLinearTransferFunction(Device &device, const LinearTransferFunctionSettings &settings)
{
    auto handle = ospNewTransferFunction("piecewiseLinear");
    auto transferFunction = wrapObjectHandleAs<LinearTransferFunction>(device, handle);

    setObjectParam(handle, "value", settings.scalarRange);

    auto stride = static_cast<std::ptrdiff_t>(sizeof(Color4));

    auto colorCount = settings.colors.getTotalItemCount();
    auto colors = createDataView<Color3>(settings.colors, {colorCount, stride});
    setObjectParam(handle, "color", colors);

    auto offset = sizeof(Color3);
    auto opacities = createDataView<float>(settings.colors, {colorCount, stride, offset});
    setObjectParam(handle, "opacity", opacities);

    commitObject(device, handle);

    return transferFunction;
}
}
