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

#include "Object.h"

namespace brayns::experimental
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

struct DistanceLightSettings : LightSettings
{
    Vector3 direction = {0.0F, 0.0F, 1.0F};
    float angularDiameter = 0.0F;
};

void loadLightParams(OSPLight handle, const DistanceLightSettings &settings);

class DistantLight : public Light
{
public:
    using Light::Light;
};

struct SphereLightSettings : LightSettings
{
    Vector3 position = {0.0F, 0.0F, 0.0F};
    float radius = 0.0F;
};

void loadLightParams(OSPLight handle, const SphereLightSettings &settings);

class SphereLight : public Light
{
public:
    using Light::Light;
};

struct SpotLightSettings : LightSettings
{
    Vector3 position = {0.0F, 0.0F, 0.0F};
    Vector3 direction = {0.0F, 0.0F, 1.0F};
    float openingAngle = 180.0F;
    float penumbraAngle = 5.0F;
    float outerRadius = 0.0F;
    float innerRadius = 0.0F;
};

void loadLightParams(OSPLight handle, const SpotLightSettings &settings);

class SpotLight : public Light
{
public:
    using Light::Light;
};

struct QuadLightSettings : LightSettings
{
    Vector3 position = {0.0F, 0.0F, 0.0F};
    Vector3 edge1 = {1.0F, 0.0F, 0.0F};
    Vector3 edge2 = {0.0F, 1.0F, 0.0F};
};

void loadLightParams(OSPLight handle, const QuadLightSettings &settings);

class QuadLight : public Light
{
public:
    using Light::Light;
};

struct AmbientLightSettings : LightSettings
{
};

void loadLightParams(OSPLight handle, const AmbientLightSettings &settings);

class AmbientLight : public Light
{
public:
    using Light::Light;
};
}