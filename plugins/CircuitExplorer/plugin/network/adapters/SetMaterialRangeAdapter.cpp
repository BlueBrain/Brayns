/* Copyright (c) 2015-2021 EPFL/Blue Brain Project
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

#include "SetMaterialRangeAdapter.h"

#include <brayns/network/common/ExtractMaterial.h>
#include <brayns/network/common/ExtractModel.h>

namespace brayns
{
MaterialRangeProxy::MaterialRangeProxy(Scene &scene)
    : _scene(&scene)
{
}

void MaterialRangeProxy::setModelId(size_t id)
{
    _model = &ExtractModel::fromId(*_scene, id);
}

void MaterialRangeProxy::setMaterialIds(const std::vector<size_t> &ids)
{
    if (!_model)
    {
        return;
    }
    if (ids.empty())
    {
        _loadAllMaterials();
        return;
    }
    _loadMaterialIds(ids);
}

void MaterialRangeProxy::setProperties(const JsonBuffer<ExtendedMaterial> &properties)
{
    _properties = properties;
}

void MaterialRangeProxy::commit() const
{
    for (auto material : _materials)
    {
        _properties.deserialize(material);
        material.commit();
    }
}

void MaterialRangeProxy::_loadAllMaterials()
{
    auto &model = _model->getModel();
    auto &materials = model.getMaterials();
    for (const auto &pair : materials)
    {
        auto &material = *pair.second;
        _materials.push_back(material);
    }
}

void MaterialRangeProxy::_loadMaterialIds(const std::vector<size_t> &ids)
{
    _materials.clear();
    _materials.reserve(ids.size());
    for (auto id : ids)
    {
        auto &material = ExtractMaterial::fromId(*_model, id);
        _materials.push_back(material);
    }
}
} // namespace brayns
