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

namespace brayns::experimental
{
void loadGroupParams(OSPGroup handle, const GroupSettings &settings)
{
    setObjectDataIfNotEmpty(handle, "geometry", settings.geometries);
    setObjectDataIfNotEmpty(handle, "volume", settings.volumes);
    setObjectDataIfNotEmpty(handle, "clippingGeometry", settings.clippingGeometries);
    setObjectDataIfNotEmpty(handle, "light", settings.lights);
    commitObject(handle);
}

Box3 Group::getBounds() const
{
    auto handle = getHandle();
    return getObjectBounds(handle);
}

void loadInstanceParams(OSPInstance handle, const InstanceSettings &settings)
{
    setObjectParam(handle, "group", settings.group);
    setObjectParam(handle, "transform", toAffine(settings.transform));
    setObjectParam(handle, "id", settings.id);
    commitObject(handle);
}

Box3 Instance::getBounds() const
{
    auto handle = getHandle();
    return getObjectBounds(handle);
}

void loadWorldParams(OSPWorld handle, const WorldSettings &settings)
{
    setObjectData(handle, "instance", settings.instances);
    commitObject(handle);
}

Box3 World::getBounds() const
{
    auto handle = getHandle();
    return getObjectBounds(handle);
}
}
