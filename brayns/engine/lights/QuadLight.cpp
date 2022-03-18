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

#include <brayns/engine/lights/QuadLight.h>

namespace brayns
{
void QuadLight::setBottomLeftCorner(const Vector3f &pos) noexcept
{
    _updateValue(_bottomLeftCorner, pos);
}

void QuadLight::setVerticalDisplacement(const Vector3f &verticalVector) noexcept
{
    _updateValue(_verticalDisplacement, verticalVector);
}

void QuadLight::setHorizontalDisplacement(const Vector3f &horizontalVector) noexcept
{
    _updateValue(_horizontalDisplacement, horizontalVector);
}

const Vector3f &QuadLight::getBottomLeftCorner() const noexcept
{
    return _bottomLeftCorner;
}

const Vector3f &QuadLight::getVerticalDisplacement() const noexcept
{
    return _verticalDisplacement;
}

const Vector3f &QuadLight::getHorizontalDisplacement() const noexcept
{
    return _horizontalDisplacement;
}

std::string_view QuadLight::getOSPHandleName() const noexcept
{
    return "quad";
}

uint64_t QuadLight::getSizeInBytes() const noexcept
{
    // We copy all the light data to OSPRay, so we must account for it
    return sizeof(QuadLight) * 2 - sizeof(OSPLight);
}

void QuadLight::commitLightSpecificParams()
{
    auto ospHandle = handle();

    ospSetParam(ospHandle, "position", OSPDataType::OSP_VEC3F, &_bottomLeftCorner);
    ospSetParam(ospHandle, "edge1", OSPDataType::OSP_VEC3F, &_horizontalDisplacement);
    ospSetParam(ospHandle, "edge2", OSPDataType::OSP_VEC3F, &_verticalDisplacement);
}
}
