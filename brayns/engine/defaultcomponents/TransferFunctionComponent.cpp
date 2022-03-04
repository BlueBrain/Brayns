/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include <brayns/engine/defaultcomponents/TransferFunctionComponent.h>

namespace brayns
{
uint64_t TransferFunctionComponent::getSizeInBytes() const noexcept
{
    const auto& colors = _transferFunction.getColors();
    const auto& opacities = _transferFunction.getOpacities();
    const auto& controlPoints = _transferFunction.getControlPoints();

    uint64_t result = sizeof(TransferFunction);
    result += colors.size() * sizeof(Vector3f);
    result += opacities.size() * sizeof(float);
    result += controlPoints.size() * sizeof(Vector2f);

    return result;
}

TransferFunction &TransferFunctionComponent::getTransferFunction() noexcept
{
    return _transferFunction;
}

void TransferFunctionComponent::setTransferFunction(TransferFunction tf) noexcept
{
    _transferFunction = std::move(tf);
}
}
