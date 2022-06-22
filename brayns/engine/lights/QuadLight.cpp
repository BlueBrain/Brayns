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

#include <brayns/engine/common/MathTypesOsprayTraits.h>

namespace
{
struct QuadLightParameters
{
    inline static const std::string osprayName = "quad";
    inline static const std::string position = "position";
    inline static const std::string edge1 = "edge1";
    inline static const std::string edge2 = "edge2";
};
}

namespace brayns
{
QuadLight::QuadLight()
    : Light(QuadLightParameters::osprayName)
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
    const auto &osprayLight = getOsprayLight();
    osprayLight.setParam(QuadLightParameters::position, _position);
    osprayLight.setParam(QuadLightParameters::edge1, _edge1);
    osprayLight.setParam(QuadLightParameters::edge2, _edge2);
}
}
