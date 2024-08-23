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

namespace brayns
{
Box3 Group::getBounds() const
{
    auto handle = getHandle();
    return getObjectBounds(handle);
}

Group createGroup(Device &device, const GroupSettings &settings)
{
    auto handle = ospNewGroup();
    auto group = wrapObjectHandleAs<Group>(device, handle);

    setObjectParam(handle, "geometry", settings.geometries);
    setObjectParam(handle, "clippingGeometry", settings.clippingGeometries);
    setObjectParam(handle, "volume", settings.volumes);
    setObjectParam(handle, "light", settings.lights);

    commitObject(device, handle);

    return group;
}

Box3 Instance::getBounds() const
{
    auto handle = getHandle();
    return getObjectBounds(handle);
}

Instance createInstance(Device &device, const InstanceSettings &settings)
{
    auto handle = ospNewInstance();
    auto instance = wrapObjectHandleAs<Instance>(device, handle);

    setObjectParam(handle, "group", settings.group);
    setObjectParam(handle, "transform", toAffine(settings.transform));
    setObjectParam(handle, "id", settings.id);

    commitObject(device, handle);

    return instance;
}

Box3 World::getBounds() const
{
    auto handle = getHandle();
    return getObjectBounds(handle);
}

World createWorld(Device &device, const WorldSettings &settings)
{
    auto handle = ospNewWorld();
    auto world = wrapObjectHandleAs<World>(device, handle);

    setObjectParam(handle, "instance", settings.instances);

    commitObject(device, handle);

    return world;
}
}
