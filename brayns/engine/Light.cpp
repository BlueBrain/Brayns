/* Copyright 2015-2024 Blue Brain Project/EPFL
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

Light::Light(LightType type)
    : _type(type)
    , _color(0.0)
    , _intensity(0.0)
    , _isVisible(false)
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

DirectionalLight::DirectionalLight()
    : Light(LightType::DIRECTIONAL)
    , _direction(0.0)
    , _angularDiameter(0.0)
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

SphereLight::SphereLight()
    : Light(LightType::SPHERE)
    , _position(0.0)
    , _radius(0.0)
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

QuadLight::QuadLight()
    : Light(LightType::QUAD)
    , _position(0.0)
    , _edge1(0.0)
    , _edge2(0.0)
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

SpotLight::SpotLight()
    : Light(LightType::SPOTLIGHT)
    , _position(0.0)
    , _direction(0.0)
    , _openingAngle(0.0)
    , _penumbraAngle(0.0)
    , _radius(0.0)
{
}

AmbientLight::AmbientLight(const Vector3d& color, double intensity,
                           bool isVisible)
    : Light(LightType::AMBIENT, color, intensity, isVisible)
{
}

AmbientLight::AmbientLight()
    : Light(LightType::AMBIENT)
{
}
} // namespace brayns
