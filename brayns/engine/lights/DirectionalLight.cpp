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

#include "DirectionalLight.h"

#include <brayns/engine/common/MathTypesOsprayTraits.h>

namespace
{
struct DirectionalLightParameters
{
    inline static const std::string osprayName = "distant";
    inline static const std::string direction = "direction";
    inline static const std::string angularDiameter = "angularDiameter";
};
}

namespace brayns
{
DirectionalLight::DirectionalLight()
    : Light(DirectionalLightParameters::osprayName)
{
}

void DirectionalLight::setDirection(const Vector3f &newDirection)
{
    if (glm::length2(newDirection) == 0.f)
    {
        throw std::invalid_argument("DirectionalLight direction must be a non-zero vector");
    }

    getModifiedFlag().update(_direction, glm::normalize(newDirection));
}

const Vector3f &DirectionalLight::getDirection() const noexcept
{
    return _direction;
}

void DirectionalLight::commitLightSpecificParams()
{
    constexpr float angularDiameter = 0.53f;

    const auto &osprayLight = getOsprayLight();
    osprayLight.setParam(DirectionalLightParameters::direction, _direction);
    osprayLight.setParam(DirectionalLightParameters::angularDiameter, angularDiameter);
}
}
