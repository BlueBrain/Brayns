/* Copyright (c) 2015-2023 EPFL/Blue Brain Project
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

#pragma once

#include <brayns/engine/scene/Scene.h>
#include <brayns/json/JsonAdapterMacro.h>

#include "BoundsAdapter.h"
#include "ModelInstanceAdapter.h"

namespace brayns
{
class SceneProxy
{
public:
    explicit SceneProxy(const Scene *scene = nullptr)
        : _scene(scene)
    {
    }

    Bounds getBounds() const noexcept
    {
        assert(_scene);
        return _scene->getBounds();
    }

    const std::vector<std::unique_ptr<ModelInstance>> &getAllInstances() const noexcept
    {
        assert(_scene);
        return _scene->getModels().getAllModelInstances();
    }

private:
    const Scene *_scene;
};

BRAYNS_JSON_ADAPTER_BEGIN(SceneProxy)
BRAYNS_JSON_ADAPTER_GET("bounds", getBounds, "Scene bounds")
BRAYNS_JSON_ADAPTER_GET("models", getAllInstances, "Scene models")
BRAYNS_JSON_ADAPTER_END()
}
