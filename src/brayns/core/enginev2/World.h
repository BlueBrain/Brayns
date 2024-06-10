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

#include "Data.h"
#include "GeometricModel.h"
#include "Light.h"
#include "Object.h"
#include "VolumetricModel.h"

namespace brayns::experimental
{
struct GroupSettings
{
    std::span<GeometricModel> geometries = {};
    std::span<VolumetricModel> volumes = {};
    std::span<ClippingModel> clippingGeometries = {};
    std::span<Light> lights = {};
};

class Group : public Managed<OSPGroup>
{
public:
    using Managed::Managed;

    Box3 getBounds() const;
};

template<>
struct ObjectReflector<Group>
{
    using Settings = GroupSettings;

    static OSPGroup createHandle(OSPDevice device, const Settings &settings);
};

struct InstanceSettings
{
    Group group;
    Transform transform = {};
    std::uint32_t id = std::uint32_t(-1);
};

class Instance : public Managed<OSPInstance>
{
public:
    using Managed::Managed;

    Box3 getBounds() const;
};

template<>
struct ObjectReflector<Instance>
{
    using Settings = InstanceSettings;

    static OSPInstance createHandle(OSPDevice device, const Settings &settings);
};

struct WorldSettings
{
    std::span<Instance> instances;
};

class World : public Managed<OSPWorld>
{
public:
    using Managed::Managed;

    Box3 getBounds() const;
};

template<>
struct ObjectReflector<World>
{
    using Settings = WorldSettings;

    static OSPWorld createHandle(OSPDevice device, const Settings &settings);
};
}
