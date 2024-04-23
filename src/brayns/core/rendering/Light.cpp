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

#include "Light.h"

#include <ospray/ospray_cpp/ext/rkcommon.h>

namespace brayns
{
void BaseLight::setColor(const Color3 &color)
{
    setParam("color", color);
}

void BaseLight::setIntensity(float intensity)
{
    setParam("intensity", intensity);
}

void BaseLight::setVisible(bool visible)
{
    setParam("visible", visible);
}

void DistantLight::setDirection(const Vector3 &direction)
{
    setParam("direction", direction);
}

void DistantLight::setAngularDiameter(float degrees)
{
    setParam("angularDiameter", degrees);
}

void SphereLight::setPosition(const Vector3 &position)
{
    setParam("position", position);
}

void SphereLight::setRadius(float radius)
{
    setParam("radius", radius);
}

void SpotLight::setPosition(const Vector3 &position)
{
    setParam("position", position);
}

void SpotLight::setDirection(const Vector3 &direction)
{
    setParam("direction", direction);
}

void SpotLight::setOpeningAngle(float degrees)
{
    setParam("openingAngle", degrees);
}

void SpotLight::setPenumbraAngle(float degrees)
{
    setParam("penumbraAngle", degrees);
}

void SpotLight::setOuterRadius(float radius)
{
    setParam("radius", radius);
}

void SpotLight::setInnerRadius(float radius)
{
    setParam("innerRadius", radius);
}

void QuadLight::setPosition(const Vector3 &position)
{
    setParam("position", position);
}

void QuadLight::setEdge1(const Vector3 &edge)
{
    setParam("edge1", edge);
}

void QuadLight::setEdge2(const Vector3 &edge)
{
    setParam("edge2", edge);
}
}
