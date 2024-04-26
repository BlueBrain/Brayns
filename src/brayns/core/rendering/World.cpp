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
void Group::setVolumes(SharedArray<VolumetricModel> models)
{
    setParam("volume", toSharedData(models));
}

void Group::setGeometries(SharedArray<GeometricModel> models)
{
    setParam("geometry", toSharedData(models));
}

void Group::setClippingGeometries(SharedArray<GeometricModel> models)
{
    setParam("clippingGeometry", toSharedData(models));
}

void Group::setLights(SharedArray<Light> lights)
{
    setParam("light", toSharedData(lights));
}

void Instance::setGroup(const Group &group)
{
    setParam("group", group.getHandle());
}

void Instance::setTransform(const Affine3 &transform)
{
    setParam("transform", transform);
}

void Instance::setId(std::uint32_t id)
{
    setParam("id", id);
}

void World::setInstances(SharedArray<Instance> instances)
{
    setParam("instance", toSharedData(instances));
}
}
