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

#include "Device.h"
#include "Object.h"
#include "Texture.h"

namespace brayns
{
struct LightSettings
{
    Color3 color = {1.0F, 1.0F, 1.0F};
    float intensity = 1.0F;
    bool visible = true;
};

class Light : public Managed<OSPLight>
{
public:
    using Managed::Managed;
};

struct DistantLightSettings
{
    LightSettings base = {};
    Vector3 direction = {0.0F, 0.0F, 1.0F};
    float angularDiameter = 0.0F;
};

class DistantLight : public Light
{
public:
    using Light::Light;
};

DistantLight createDistantLight(Device &device, const DistantLightSettings &settings);

struct SphereLightSettings
{
    LightSettings base = {};
    Vector3 position = {0.0F, 0.0F, 0.0F};
    float radius = 0.0F;
};

class SphereLight : public Light
{
public:
    using Light::Light;
};

SphereLight createSphereLight(Device &device, const SphereLightSettings &settings);

struct SpotLightSettings
{
    LightSettings base = {};
    Vector3 position = {0.0F, 0.0F, 0.0F};
    Vector3 direction = {0.0F, 0.0F, 1.0F};
    float openingAngle = 180.0F;
    float penumbraAngle = 5.0F;
    float outerRadius = 0.0F;
    float innerRadius = 0.0F;
};

class SpotLight : public Light
{
public:
    using Light::Light;
};

SpotLight createSpotLight(Device &device, const SpotLightSettings &settings);

struct QuadLightSettings
{
    LightSettings base = {};
    Vector3 position = {0.0F, 0.0F, 0.0F};
    Vector3 edge1 = {1.0F, 0.0F, 0.0F};
    Vector3 edge2 = {0.0F, 1.0F, 0.0F};
};

class QuadLight : public Light
{
public:
    using Light::Light;
};

QuadLight createQuadLight(Device &device, const QuadLightSettings &settings);

struct CylinderLightSettings
{
    LightSettings base = {};
    Vector3 start = {0.0F, 0.0F, 0.0F};
    Vector3 end = {0.0F, 0.0F, 1.0F};
    float radius = 1.0F;
};

class CylinderLight : public Light
{
public:
    using Light::Light;
};

CylinderLight createCylinderLight(Device &device, const CylinderLightSettings &settings);

struct HdriLightSettings
{
    Texture2D map;
    LightSettings base = {};
    Vector3 up = {0.0F, 1.0F, 0.0F};
    Vector3 direction = {0.0F, 0.0F, 1.0F};
};

class HdriLight : public Light
{
public:
    using Light::Light;
};

HdriLight createHdriLight(Device &device, const HdriLightSettings &settings);

struct AmbientLightSettings
{
    LightSettings base = {};
};

class AmbientLight : public Light
{
public:
    using Light::Light;
};

AmbientLight createAmbientLight(Device &device, const AmbientLightSettings &settings);

struct SunSkyLightSettings
{
    LightSettings base = {};
    Vector3 up = {0.0F, 1.0F, 0.0F};
    Vector3 direction = {0.0F, -1.0F, 0.0F};
    float turbidity = 3.0F;
    float albedo = 0.3F;
    float horizonExtension = 0.01F;
};

class SunSkyLight : public Light
{
public:
    using Light::Light;
};

SunSkyLight createSunSkyLight(Device &device, const SunSkyLightSettings &settings);
}
