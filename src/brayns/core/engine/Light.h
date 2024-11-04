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
    Vector3 direction = {0.0F, 0.0F, 1.0F};
    float angularDiameter = 0.0F;
};

class DistantLight : public Light
{
public:
    using Light::Light;
};

DistantLight createDistantLight(Device &device, const LightSettings &settings = {}, const DistantLightSettings &distant = {});

struct SphereLightSettings
{
    Vector3 position = {0.0F, 0.0F, 0.0F};
    float radius = 0.0F;
};

class SphereLight : public Light
{
public:
    using Light::Light;
};

SphereLight createSphereLight(Device &device, const LightSettings &settings = {}, const SphereLightSettings &sphere = {});

struct SpotLightSettings
{
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

SpotLight createSpotLight(Device &device, const LightSettings &settings = {}, const SpotLightSettings &spot = {});

struct QuadLightSettings
{
    Vector3 position = {0.0F, 0.0F, 0.0F};
    Vector3 edge1 = {1.0F, 0.0F, 0.0F};
    Vector3 edge2 = {0.0F, 1.0F, 0.0F};
};

class QuadLight : public Light
{
public:
    using Light::Light;
};

QuadLight createQuadLight(Device &device, const LightSettings &settings = {}, const QuadLightSettings &quad = {});

struct CylinderLightSettings
{
    Vector3 start = {0.0F, 0.0F, 0.0F};
    Vector3 end = {0.0F, 0.0F, 1.0F};
    float radius = 1.0F;
};

class CylinderLight : public Light
{
public:
    using Light::Light;
};

CylinderLight createCylinderLight(Device &device, const LightSettings &settings = {}, const CylinderLightSettings &cylinder = {});

struct HdriLightSettings
{
    Texture2D map;
    Vector3 up = {0.0F, 1.0F, 0.0F};
    Vector3 direction = {0.0F, 0.0F, 1.0F};
};

class HdriLight : public Light
{
public:
    using Light::Light;
};

HdriLight createHdriLight(Device &device, const LightSettings &settings, const HdriLightSettings &hdri);

class AmbientLight : public Light
{
public:
    using Light::Light;
};

AmbientLight createAmbientLight(Device &device, const LightSettings &settings = {});

struct SunSkyLightSettings
{
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

SunSkyLight createSunSkyLight(Device &device, const LightSettings &settings = {}, const SunSkyLightSettings &sunsky = {});
}
