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

#include <variant>

#include "Device.h"
#include "Object.h"
#include "Texture.h"

namespace brayns
{
struct LightSettings
{
    Color3 color = {1.0F, 1.0F, 1.0F};
    bool visible = true;
};

struct Power
{
    float value = 1.0F;
};

struct Intensity
{
    float value = 1.0F;
};

struct Radiance
{
    float value = 1.0F;
};

struct Irradiance
{
    float value = 1.0F;
};

class Light : public Managed<OSPLight>
{
public:
    using Managed::Managed;

    void update(const LightSettings &settings);
};

struct DistantLightSettings
{
    Vector3 direction = {0.0F, 0.0F, 1.0F};
    float angularDiameter = radians(0.0F);
    std::variant<Radiance, Irradiance> intensity = Irradiance{};
};

class DistantLight : public Light
{
public:
    using Light::Light;
    using Light::update;

    void update(const DistantLightSettings &settings);
};

DistantLight createDistantLight(Device &device, const LightSettings &settings = {}, const DistantLightSettings &distant = {});

struct SphereLightSettings
{
    Vector3 position = {0.0F, 0.0F, 0.0F};
    float radius = 0.0F;
    std::variant<Power, Intensity, Radiance> intensity = Intensity{};
};

class SphereLight : public Light
{
public:
    using Light::Light;
    using Light::update;

    void update(const SphereLightSettings &settings);
};

SphereLight createSphereLight(Device &device, const LightSettings &settings = {}, const SphereLightSettings &sphere = {});

struct SpotLightSettings
{
    Vector3 position = {0.0F, 0.0F, 0.0F};
    Vector3 direction = {0.0F, 0.0F, 1.0F};
    float openingAngle = radians(180.0F);
    float penumbraAngle = radians(5.0F);
    float outerRadius = 0.0F;
    float innerRadius = 0.0F;
    std::variant<Power, Intensity, Radiance> intensity = Intensity{};
};

class SpotLight : public Light
{
public:
    using Light::Light;
    using Light::update;

    void update(const SpotLightSettings &settings);
};

SpotLight createSpotLight(Device &device, const LightSettings &settings = {}, const SpotLightSettings &spot = {});

struct QuadLightSettings
{
    Vector3 position = {0.0F, 0.0F, 0.0F};
    Vector3 edge1 = {1.0F, 0.0F, 0.0F};
    Vector3 edge2 = {0.0F, 1.0F, 0.0F};
    std::variant<Power, Intensity, Radiance> intensity = Radiance{};
};

class QuadLight : public Light
{
public:
    using Light::Light;
    using Light::update;

    void update(const QuadLightSettings &settings);
};

QuadLight createQuadLight(Device &device, const LightSettings &settings = {}, const QuadLightSettings &quad = {});

struct CylinderLightSettings
{
    Vector3 start = {0.0F, 0.0F, 0.0F};
    Vector3 end = {0.0F, 0.0F, 1.0F};
    float radius = 1.0F;
    std::variant<Power, Intensity, Radiance> intensity = Radiance{};
};

class CylinderLight : public Light
{
public:
    using Light::Light;
    using Light::update;

    void update(const CylinderLightSettings &settings);
};

CylinderLight createCylinderLight(Device &device, const LightSettings &settings = {}, const CylinderLightSettings &cylinder = {});

struct HdriLightSettings
{
    Vector3 up = {0.0F, 1.0F, 0.0F};
    Vector3 direction = {0.0F, 0.0F, 1.0F};
    float intensityScale = 1.0F;
};

class HdriLight : public Light
{
public:
    using Light::Light;
    using Light::update;

    void update(const Texture2D &map, const HdriLightSettings &settings);
};

HdriLight createHdriLight(Device &device, const Texture2D &map, const LightSettings &settings = {}, const HdriLightSettings &hdri = {});

struct AmbientLightSettings
{
    std::variant<Radiance, Irradiance> intensity = Irradiance{};
};

class AmbientLight : public Light
{
public:
    using Light::Light;
    using Light::update;

    void update(const AmbientLightSettings &settings);
};

AmbientLight createAmbientLight(Device &device, const LightSettings &settings = {}, const AmbientLightSettings &ambient = {});

struct SunSkyLightSettings
{
    Vector3 up = {0.0F, 1.0F, 0.0F};
    Vector3 direction = {0.0F, -1.0F, 0.0F};
    float turbidity = 3.0F;
    float albedo = 0.3F;
    float horizonExtension = 0.01F;
    float intensityScale = 1.0F;
};

class SunSkyLight : public Light
{
public:
    using Light::Light;
    using Light::update;

    void update(const SunSkyLightSettings &settings);
};

SunSkyLight createSunSkyLight(Device &device, const LightSettings &settings = {}, const SunSkyLightSettings &sunsky = {});
}
