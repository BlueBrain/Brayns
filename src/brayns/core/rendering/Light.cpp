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

#include <string>

#include <ospray/ospray_cpp/ext/rkcommon.h>

namespace
{
const std::string colorKey = "color";
const std::string intensityKey = "intensity";
const std::string visibleKey = "visible";

const std::string directionKey = "direction";
const std::string angularDiameterKey = "angularDiameter";

const std::string positionKey = "position";
const std::string radiusKey = "radius";

const std::string openingAngleKey = "openingAngle";
const std::string penumbraAngleKey = "penumbraAngle";
const std::string innerRadiusKey = "innerRadius";

const std::string edge1Key = "edge1";
const std::string edge2Key = "edge2";
}

namespace brayns
{
BaseLight::BaseLight(ospray::cpp::Light light):
    _light(std::move(light))
{
}

ospray::cpp::Light BaseLight::getHandle() const
{
    return _light;
}

void BaseLight::setColor(const Color3 &color)
{
    _light.setParam(colorKey, color);
}

void BaseLight::setIntensity(float intensity)
{
    _light.setParam(intensityKey, intensity);
}

void BaseLight::setVisible(bool visible)
{
    _light.setParam(visibleKey, visible);
}

void BaseLight::commit()
{
    _light.commit();
}

void DistantLight::setDirection(const Vector3 &direction)
{
    _light.setParam(directionKey, direction);
}

void DistantLight::setAngularDiameter(float degrees)
{
    _light.setParam(angularDiameterKey, degrees);
}

void SphereLight::setPosition(const Vector3 &position)
{
    _light.setParam(positionKey, position);
}

void SphereLight::setRadius(float radius)
{
    _light.setParam(radiusKey, radius);
}

void SpotLight::setPosition(const Vector3 &position)
{
    _light.setParam(positionKey, position);
}

void SpotLight::setDirection(const Vector3 &direction)
{
    _light.setParam(directionKey, direction);
}

void SpotLight::setOpeningAngle(float degrees)
{
    _light.setParam(openingAngleKey, degrees);
}

void SpotLight::setPenumbraAngle(float degrees)
{
    _light.setParam(penumbraAngleKey, degrees);
}

void SpotLight::setOuterRadius(float radius)
{
    _light.setParam(radiusKey, radius);
}

void SpotLight::setInnerRadius(float radius)
{
    _light.setParam(innerRadiusKey, radius);
}

void QuadLight::setPosition(const Vector3 &position)
{
    _light.setParam(positionKey, position);
}

void QuadLight::setEdge1(const Vector3 &edge)
{
    _light.setParam(edge1Key, edge);
}

void QuadLight::setEdge2(const Vector3 &edge)
{
    _light.setParam(edge2Key, edge);
}
}
