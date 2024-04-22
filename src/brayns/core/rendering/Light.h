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

#pragma once

#include <ospray/ospray_cpp.h>

#include <brayns/core/utils/Math.h>

namespace brayns
{
class BaseLight
{
public:
    explicit BaseLight(ospray::cpp::Light light);

    ospray::cpp::Light getHandle() const;
    void setColor(const Color3 &color);
    void setIntensity(float intensity);
    void setVisible(bool visible);
    void commit();

protected:
    ospray::cpp::Light _light;
};

class DistantLight : public BaseLight
{
public:
    using BaseLight::BaseLight;

    void setDirection(const Vector3 &direction);
    void setAngularDiameter(float degrees);
};

class SphereLight : public BaseLight
{
public:
    using BaseLight::BaseLight;

    void setPosition(const Vector3 &position);
    void setRadius(float radius);
};

class SpotLight : public BaseLight
{
public:
    using BaseLight::BaseLight;

    void setPosition(const Vector3 &position);
    void setDirection(const Vector3 &direction);
    void setOpeningAngle(float degrees);
    void setPenumbraAngle(float degrees);
    void setOuterRadius(float radius);
    void setInnerRadius(float radius);
};

class QuadLight : public BaseLight
{
public:
    using BaseLight::BaseLight;

    void setPosition(const Vector3 &position);
    void setEdge1(const Vector3 &edge);
    void setEdge2(const Vector3 &edge);
};

class AmbientLight : public BaseLight
{
public:
    using BaseLight::BaseLight;
};
}
