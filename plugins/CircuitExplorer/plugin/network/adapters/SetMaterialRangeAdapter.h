/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#include <brayns/engine/Material.h>
#include <brayns/engine/Model.h>
#include <brayns/engine/Scene.h>
#include <brayns/json/JsonAdapterMacro.h>
#include <brayns/json/JsonBuffer.h>
#include <brayns/network/common/ExtractMaterial.h>
#include <brayns/network/common/ExtractModel.h>

#include "MaterialAdapter.h"

namespace brayns
{
class MaterialRangeProxy
{
public:
    MaterialRangeProxy() = default;

    MaterialRangeProxy(Scene& scene)
        : _scene(&scene)
    {
    }

    void setModelId(size_t id) { _model = &ExtractModel::fromId(*_scene, id); }

    void setMaterialIds(const std::vector<size_t>& ids)
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

    void setProperties(const JsonBuffer<ExtendedMaterial>& properties)
    {
        _properties = properties;
    }

    void commit() const
    {
        for (auto material : _materials)
        {
            _properties.deserialize(material);
            material.commit();
        }
    }

private:
    void _loadAllMaterials()
    {
        auto& model = _model->getModel();
        auto& materials = model.getMaterials();
        for (const auto& pair : materials)
        {
            auto& material = *pair.second;
            _materials.push_back(material);
        }
    }

    void _loadMaterialIds(const std::vector<size_t>& ids)
    {
        _materials.clear();
        _materials.reserve(ids.size());
        for (auto id : ids)
        {
            auto& material = ExtractMaterial::fromId(*_model, id);
            _materials.push_back(material);
        }
    }

    Scene* _scene = nullptr;
    ModelDescriptor* _model = nullptr;
    std::vector<ExtendedMaterial> _materials;
    JsonBuffer<ExtendedMaterial> _properties;
};

BRAYNS_NAMED_JSON_ADAPTER_BEGIN(MaterialRangeProxy, "MaterialRange")
BRAYNS_JSON_ADAPTER_SET("model_id", setModelId,
                        "The model which this material belongs to")
BRAYNS_JSON_ADAPTER_SET("material_ids", setMaterialIds,
                        "The list of ID that identifies the materials")
BRAYNS_JSON_ADAPTER_SET("properties", setProperties,
                        "Material properties to apply on all given materials")
BRAYNS_JSON_ADAPTER_END()
} // namespace brayns
