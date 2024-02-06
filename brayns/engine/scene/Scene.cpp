/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *                     Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include <ospray/ospray_cpp/Data.h>

namespace
{
struct WorldParameters
{
    static inline const std::string instance = "instance";
};

class WorldInstances
{
public:
    static void set(ospray::cpp::World &world, const std::vector<ospray::cpp::Instance> &instances)
    {
        if (instances.empty())
        {
            world.removeParam(WorldParameters::instance);
            return;
        }
        world.setParam(WorldParameters::instance, ospray::cpp::CopiedData(instances));
    }
};
}

namespace brayns
{
Bounds Scene::getBounds() const noexcept
{
    return _models.getBounds();
}

void Scene::update(const ParametersManager &params)
{
    _models.update(params);
}

bool Scene::commit()
{
    auto modelCommitResult = _models.commit();

    if (modelCommitResult.needsRebuildBVH)
    {
        WorldInstances::set(_handle, _models.getHandles());
        _handle.commit();
    }

    return modelCommitResult.needsRebuildBVH || modelCommitResult.needsRender;
}

ModelManager &Scene::getModels() noexcept
{
    return _models;
}

const ModelManager &Scene::getModels() const noexcept
{
    return _models;
}

const ospray::cpp::World &Scene::getHandle() const noexcept
{
    return _handle;
}
} // namespace brayns
