/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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

#include "SceneClipManager.h"

namespace brayns
{
uint32_t SceneClipManager::addClippingModel(std::unique_ptr<Model> clippingModel) noexcept
{
    const auto id = _idFactory.generateID();

    auto &clippingEntry = _clippingModels[id];

    clippingEntry.model = std::move(clippingModel);
    auto &model = *clippingEntry.model;

    clippingEntry.instance = std::make_unique<ModelInstance>(0, model);
    auto &instance = *clippingEntry.instance;

    model.commit();
    instance.commit(true);
    _dirty = true;
    return id;
}

void SceneClipManager::removeClippingModel(const uint32_t id)
{
    auto eraseCount = _clippingModels.erase(id);
    if (eraseCount == 0)
    {
        throw std::invalid_argument("No clipping model with ID " + std::to_string(id) + " exists");
    }

    _idFactory.releaseID(id);
    _dirty = true;
}

void SceneClipManager::removeAllClippingModels() noexcept
{
    _idFactory.clear();
    _clippingModels.clear();
    _dirty = true;
}

bool SceneClipManager::commit()
{
    auto result = _dirty;
    _dirty = false;
    return result;
}

std::vector<OSPInstance> SceneClipManager::getInstanceHandles() noexcept
{
    std::vector<OSPInstance> result;
    result.reserve(_clippingModels.size());
    for (auto &[id, entry] : _clippingModels)
    {
        auto &instance = *entry.instance;
        auto handle = instance.handle();
        result.push_back(handle);
    }

    return result;
}

}
