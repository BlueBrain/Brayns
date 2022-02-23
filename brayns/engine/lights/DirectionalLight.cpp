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

#include <brayns/engine/lights/DirectionalLight.h>

namespace brayns
{
void DirectionalLight::setDirection(const Vector3f& newDirection)
{
    if(glm::length2(newDirection) == 0.f)
        throw std::invalid_argument("DirectionalLight direction must be a non-zero vector");

    _updateValue(_direction, glm::normalize(newDirection));
}

const Vector3f& DirectionalLight::getDirection() const noexcept
{
    return _direction;
}

std::string_view DirectionalLight::getName() const noexcept
{
    return "directional";
}

uint64_t DirectionalLight::getSizeInBytes() const noexcept
{
    return sizeof(DirectionalLight);
}

void DirectionalLight::commitLightSpecificParams()
{
    auto ospHandle = handle();

    // Only taken into account by stochastic sampling renderers.
    // Value >= 0.f allows for soft shadows. 0.f will produce hard shadows
    static constexpr float angularDiameter = 0.53f;

    ospSetParam(ospHandle, "direction", OSPDataType::OSP_VEC3F, &_direction);
    ospSetParam(ospHandle, "angularDiameter", OSPDataType::OSP_FLOAT, &angularDiameter);
}

std::string_view DirectionalLight::getOSPHandleName() const noexcept
{
    return "distant";
}
}
