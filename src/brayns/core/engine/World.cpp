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

Box3 Instance::getBounds() const
{
    auto handle = getHandle();
    return getObjectBounds(handle);
}

Box3 World::getBounds() const
{
    auto handle = getHandle();
    return getObjectBounds(handle);
}

OSPGroup ObjectReflector<Group>::createHandle(OSPDevice device, const Settings &settings)
{
    auto handle = ospNewGroup();
    throwLastDeviceErrorIfNull(device, handle);

    setObjectDataIfNotEmpty(handle, "geometry", settings.geometries);
    setObjectDataIfNotEmpty(handle, "volume", settings.volumes);
    setObjectDataIfNotEmpty(handle, "clippingGeometry", settings.clippingGeometries);
    setObjectDataIfNotEmpty(handle, "light", settings.lights);

    commitObject(handle);

    return handle;
}

OSPInstance ObjectReflector<Instance>::createHandle(OSPDevice device, const Settings &settings)
{
    auto handle = ospNewInstance();
    throwLastDeviceErrorIfNull(device, handle);

    setObjectParam(handle, "group", settings.group);
    setObjectParam(handle, "transform", toAffine(settings.transform));
    setObjectParam(handle, "id", settings.id);

    commitObject(handle);

    return handle;
}

OSPWorld ObjectReflector<World>::createHandle(OSPDevice device, const Settings &settings)
{
    auto handle = ospNewWorld();
    throwLastDeviceErrorIfNull(device, handle);

    setObjectData(handle, "instance", settings.instances);

    commitObject(handle);

    return handle;
}
}
