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
#include "Geometry.h"
#include "Light.h"
#include "Object.h"
#include "Volume.h"

namespace brayns::experimental
{
class Group : public Object<OSPGroup>
{
public:
    using Object::getBounds;
    using Object::Object;

    void setVolumes(SharedArray<VolumetricModel> models);
    void setGeometries(SharedArray<GeometricModel> models);
    void setClippingGeometries(SharedArray<GeometricModel> models);
    void setLights(SharedArray<Light> lights);
};

class Instance : public Object<OSPInstance>
{
public:
    using Object::getBounds;
    using Object::Object;

    void setGroup(const Group &group);
    void setTransform(const Affine3 &transform);
    void setId(std::uint32_t id);
};

class World : public Object<OSPWorld>
{
public:
    using Object::getBounds;
    using Object::Object;

    void setInstances(SharedArray<Instance> instances);
};
}

namespace ospray
{
OSPTYPEFOR_SPECIALIZATION(brayns::experimental::Group, OSP_GROUP)
OSPTYPEFOR_SPECIALIZATION(brayns::experimental::Instance, OSP_INSTANCE)
OSPTYPEFOR_SPECIALIZATION(brayns::experimental::World, OSP_WORLD)
}
