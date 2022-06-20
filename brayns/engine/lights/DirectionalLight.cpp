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

#include <brayns/engine/ospray/OsprayMathtypesTraits.h>

namespace
{
class DirectionalLightParameterUpdater
{
public:
    static void update(const brayns::DirectionalLight &light)
    {
        static const std::string directionParam = "direction";
        static const std::string angularDiameterParam = "angularDiameter";

        const auto &direction = light.getDirection();
        // angularDiamter = 0.0 == HARD SHADOWS
        constexpr float angularDiameter = 0.53f;

        const auto &osprayLight = light.getOsprayLight();
        osprayLight.setParam(directionParam, direction);
        osprayLight.setParam(angularDiameterParam, angularDiameter);
    }
};
}

namespace brayns
{
DirectionalLight::DirectionalLight()
    : Light("distant")
{
}

void DirectionalLight::setDirection(const Vector3f &newDirection)
{
    if (glm::length2(newDirection) == 0.f)
    {
        throw std::invalid_argument("DirectionalLight direction must be a non-zero vector");
    }

    _updateValue(_direction, glm::normalize(newDirection));
}

const Vector3f &DirectionalLight::getDirection() const noexcept
{
    return _direction;
}

void DirectionalLight::commitLightSpecificParams()
{
    DirectionalLightParameterUpdater::update(*this);
}
}
