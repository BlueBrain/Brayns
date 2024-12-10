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

#include "World.h"

namespace
{
using namespace brayns;

void setGroupParams(OSPGroup handle, const GroupSettings &settings)
{
    setObjectParam(handle, "geometry", settings.geometries);
    setObjectParam(handle, "clippingGeometry", settings.clippingGeometries);
    setObjectParam(handle, "volume", settings.volumes);
    setObjectParam(handle, "light", settings.lights);
}

void setInstanceParams(OSPInstance handle, const Group &group, const InstanceSettings &settings)
{
    setObjectParam(handle, "group", group);
    setObjectParam(handle, "transform", toAffine(settings.transform));
    setObjectParam(handle, "id", settings.id);
}

void setWorldParams(OSPWorld handle, const WorldSettings &settings)
{
    setObjectParam(handle, "instance", settings.instances);
}
}

namespace brayns
{
Box3 Group::getBounds() const
{
    auto handle = getHandle();
    return getObjectBounds(handle);
}

void Group::update(const GroupSettings &settings)
{
    auto handle = getHandle();
    setGroupParams(handle, settings);
    commitObject(handle);
}

Group createGroup(Device &device, const GroupSettings &settings)
{
    auto handle = ospNewGroup();
    auto group = wrapObjectHandleAs<Group>(device, handle);

    setGroupParams(handle, settings);

    commitObject(device, handle);

    return group;
}

Box3 Instance::getBounds() const
{
    auto handle = getHandle();
    return getObjectBounds(handle);
}

void Instance::update(const Group &group, const InstanceSettings &settings)
{
    auto handle = getHandle();
    setInstanceParams(handle, group, settings);
    commitObject(handle);
}

Instance createInstance(Device &device, const Group &group, const InstanceSettings &settings)
{
    auto handle = ospNewInstance();
    auto instance = wrapObjectHandleAs<Instance>(device, handle);

    setInstanceParams(handle, group, settings);

    commitObject(device, handle);

    return instance;
}

Box3 World::getBounds() const
{
    auto handle = getHandle();
    return getObjectBounds(handle);
}

void World::update(const WorldSettings &settings)
{
    auto handle = getHandle();
    setWorldParams(handle, settings);
    commitObject(handle);
}

World createWorld(Device &device, const WorldSettings &settings)
{
    auto handle = ospNewWorld();
    auto world = wrapObjectHandleAs<World>(device, handle);

    setWorldParams(handle, settings);

    commitObject(device, handle);

    return world;
}
}
