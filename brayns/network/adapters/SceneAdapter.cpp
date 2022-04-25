/* Copyright (c) 2015-2022 EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include "SceneAdapter.h"

namespace brayns
{
ReadSceneProxy::ReadSceneProxy(const Scene &scene)
    : _scene(&scene)
{
}

Bounds ReadSceneProxy::getBounds() const noexcept
{
    if (_scene)
    {
        return _scene->getBounds();
    }

    return {Vector3f(0.f), Vector3f(0.f)};
}

std::vector<ModelInstanceProxy> ReadSceneProxy::getModels() const noexcept
{
    if (_scene)
    {
        const auto &modelManager = _scene->getModelManager();
        auto &models = modelManager.getAllModelInstances();

        std::vector<ModelInstanceProxy> result;
        result.reserve(models.size());
        for (auto model : models)
        {
            result.emplace_back(*model);
        }
        return result;
    }

    return {};
}

}
