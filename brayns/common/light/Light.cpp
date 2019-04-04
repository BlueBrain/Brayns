/* Copyright (c) 2015-2019, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
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

#include "brayns/common/utils/utils.h"

namespace brayns
{
Light::Light(LightType type, const Vector3d& color, double intensity,
             bool isVisible)
    : _type(type)
    , _color(color)
    , _intensity(intensity)
    , _isVisible(isVisible)
{
}

DirectionalLight::DirectionalLight(const Vector3d& direction,
                                   double angularDiameter,
                                   const Vector3d& color, double intensity,
                                   bool isVisible)
    : Light(LightType::DIRECTIONAL, color, intensity, isVisible)
    , _direction(direction)
    , _angularDiameter(angularDiameter)
{
}

SphereLight::SphereLight(const Vector3d& position, double radius,
                         const Vector3d& color, double intensity,
                         bool isVisible)
    : Light(LightType::SPHERE, color, intensity, isVisible)
    , _position(position)
    , _radius(radius)
{
}

QuadLight::QuadLight(const Vector3d& position, const Vector3d& edge1,
                     const Vector3d& edge2, const Vector3d& color,
                     double intensity, bool isVisible)
    : Light(LightType::QUAD, color, intensity, isVisible)
    , _position(position)
    , _edge1(edge1)
    , _edge2(edge2)
{
}

SpotLight::SpotLight(const Vector3d& position, const Vector3d& direction,
                     const double openingAngle, const double penumbraAngle,
                     const double radius, const Vector3d& color,
                     double intensity, bool isVisible)
    : Light(LightType::SPOTLIGHT, color, intensity, isVisible)
    , _position(position)
    , _direction(direction)
    , _openingAngle(openingAngle)
    , _penumbraAngle(penumbraAngle)
    , _radius(radius)
{
}

AmbientLight::AmbientLight(const Vector3d& color, double intensity,
                           bool isVisible)
    : Light(LightType::AMBIENT, color, intensity, isVisible)
{
}

} // namespace brayns
