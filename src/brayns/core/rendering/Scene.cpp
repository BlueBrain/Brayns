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

#include "Scene.h"

#include <ospray/ospray_cpp/ext/rkcommon.h>

namespace brayns
{
void Group::setVolumes(const std::vector<ospray::cpp::VolumetricModel> &models)
{
    setParam("volume", ospray::cpp::SharedData(models));
}

void Group::setGeometries(const std::vector<ospray::cpp::GeometricModel> &models)
{
    setParam("geometry", ospray::cpp::SharedData(models));
}

void Group::setClippingGeometries(const std::vector<ospray::cpp::GeometricModel> &models)
{
    setParam("clippingGeometry", ospray::cpp::SharedData(models));
}

void Group::setLights(const std::vector<ospray::cpp::Light> &lights)
{
    setParam("light", ospray::cpp::SharedData(lights));
}

void Instance::setGroup(const ospray::cpp::Group &group)
{
    setParam("group", group);
}

void Instance::setTransform(const Affine3 &transform)
{
    setParam("transform", transform);
}

void Instance::setId(std::uint32_t id)
{
    setParam("id", id);
}

void Scene::setInstances(const std::vector<ospray::cpp::Instance> &instances)
{
    setParam("instance", ospray::cpp::SharedData(instances));
}
}
