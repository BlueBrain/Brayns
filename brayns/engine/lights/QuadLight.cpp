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

#include "QuadLight.h"

#include <brayns/engine/ospray/OsprayMathtypesTraits.h>

namespace
{
class QuadLightParameterUpdater
{
public:
    static void update(const brayns::QuadLight &light)
    {
        static const std::string positionParam = "position";
        static const std::string edge1Param = "edge1";
        static const std::string edge2Param = "edge2";

        const auto &position = light.getPosition();
        const auto &edge1 = light.getEdge1();
        const auto &edge2 = light.getEdge2();

        const auto &osprayLight = light.getOsprayLight();
        osprayLight.setParam(positionParam, position);
        osprayLight.setParam(edge1Param, edge1);
        osprayLight.setParam(edge2Param, edge2);
    }
};
}

namespace brayns
{
QuadLight::QuadLight()
    : Light("quad")
{
}

void QuadLight::setPosition(const Vector3f &position) noexcept
{
    _updateValue(_position, position);
}

void QuadLight::setEdge1(const Vector3f &edge1) noexcept
{
    _updateValue(_edge1, edge1);
}

void QuadLight::setEdge2(const Vector3f &edge2) noexcept
{
    _updateValue(_edge2, edge2);
}

const Vector3f &QuadLight::getPosition() const noexcept
{
    return _position;
}

const Vector3f &QuadLight::getEdge1() const noexcept
{
    return _edge1;
}

const Vector3f &QuadLight::getEdge2() const noexcept
{
    return _edge2;
}

Bounds QuadLight::computeBounds() const noexcept
{
    Bounds bounds;
    bounds.expand(_position);
    bounds.expand(_position + _edge1);
    bounds.expand(_position + _edge2);
    bounds.expand(_position + _edge1 + _edge2);
    return bounds;
}

void QuadLight::commitLightSpecificParams()
{
    QuadLightParameterUpdater::update(*this);
}
}
