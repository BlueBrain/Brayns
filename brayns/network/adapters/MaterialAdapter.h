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

#pragma once

#include <brayns/common/Log.h>
#include <brayns/engine/components/MaterialComponent.h>
#include <brayns/engine/materials/CarPaintMaterial.h>
#include <brayns/engine/materials/DefaultMaterial.h>
#include <brayns/engine/materials/EmissiveMaterial.h>
#include <brayns/engine/materials/GlassMaterial.h>
#include <brayns/engine/materials/MatteMaterial.h>
#include <brayns/engine/materials/MetalMaterial.h>
#include <brayns/engine/materials/PlasticMaterial.h>
#include <brayns/engine/scenecomponents/SceneModelManager.h>
#include <brayns/json/JsonAdapterMacro.h>
#include <brayns/json/JsonBuffer.h>
#include <brayns/json/JsonObjectMacro.h>

namespace brayns
{
BRAYNS_JSON_ADAPTER_BEGIN(CarPaintMaterial)
BRAYNS_JSON_ADAPTER_GETSET("color", getColor, setColor, "Base color of the material")
BRAYNS_JSON_ADAPTER_GETSET(
    "flake_density",
    getFlakesDensity,
    setFlakesDesnity,
    "Normalized percentage of flakes on the surface. Will be clampled to the range [0.0, 1.0]")
BRAYNS_JSON_ADAPTER_END()

BRAYNS_JSON_ADAPTER_BEGIN(DefaultMaterial)
BRAYNS_JSON_ADAPTER_GETSET("color", getColor, setColor, "Base color of the material")
BRAYNS_JSON_ADAPTER_GETSET(
    "opacity",
    getOpacity,
    setOpacity,
    "Base opacity of the material. Will be clampled to the range [0.0, 1.0]")
BRAYNS_JSON_ADAPTER_END()

BRAYNS_JSON_ADAPTER_BEGIN(EmissiveMaterial)
BRAYNS_JSON_ADAPTER_GETSET("color", getColor, setColor, "Base color of the material")
BRAYNS_JSON_ADAPTER_GETSET(
    "intensity",
    getIntensity,
    setIntensity,
    "Intensity of the light emitted. Will be clampled to the range [0.0, +infinite]")
BRAYNS_JSON_ADAPTER_END()

BRAYNS_JSON_ADAPTER_BEGIN(GlassMaterial)
BRAYNS_JSON_ADAPTER_GETSET("color", getColor, setColor, "Base color of the material")
BRAYNS_JSON_ADAPTER_GETSET(
    "index_of_refraction",
    getIndexOfRefraction,
    setIndexOfRefraction,
    "Index of refraction of the glass")
BRAYNS_JSON_ADAPTER_END()

BRAYNS_JSON_ADAPTER_BEGIN(MatteMaterial)
BRAYNS_JSON_ADAPTER_GETSET("color", getColor, setColor, "Base color of the material")
BRAYNS_JSON_ADAPTER_GETSET(
    "opacity",
    getOpacity,
    setOpacity,
    "Base opacity of the material. Will be clampled to the range [0.0, 1.0]")
BRAYNS_JSON_ADAPTER_END()

BRAYNS_JSON_ADAPTER_BEGIN(MetalMaterial)
BRAYNS_JSON_ADAPTER_GETSET("color", getColor, setColor, "Base color of the material")
BRAYNS_JSON_ADAPTER_GETSET(
    "roughness",
    getRoughness,
    setRoughness,
    "Surface roughness. Will be clamped on the range [0-1]")
BRAYNS_JSON_ADAPTER_END()

BRAYNS_JSON_ADAPTER_BEGIN(PlasticMaterial)
BRAYNS_JSON_ADAPTER_GETSET("color", getColor, setColor, "Base color of the material")
BRAYNS_JSON_ADAPTER_GETSET(
    "opacity",
    getOpacity,
    setOpacity,
    "Base opacity of the material. Will be clampled to the range [0.0, 1.0]")
BRAYNS_JSON_ADAPTER_END()

template<typename MaterialType>
class ModelMaterial
{
public:
    ModelMaterial() = default;
    ModelMaterial(SceneModelManager &modelManager)
        : _modelManager(&modelManager)
        , _material(std::make_unique<MaterialType>())
    {
    }

    void setModelId(const uint32_t id) noexcept
    {
        _modelId = id;
    }

    void setMaterial(const JsonBuffer<MaterialType> &material) noexcept
    {
        material.deserialize(*_material);
    }

    void update()
    {
        auto &modelInstance = _modelManager->getModelInstance(_modelId);
        auto &model = modelInstance.getModel();
        auto &materialComponent = model.getComponent<MaterialComponent>();
        materialComponent.setMaterial(std::move(_material));
    }

private:
    SceneModelManager *_modelManager{nullptr};
    uint32_t _modelId{};
    std::unique_ptr<MaterialType> _material;
};

#define MODEL_MATERIAL_PARAMS() \
    BRAYNS_JSON_ADAPTER_SET("model_id", setModelId, "ID of the model") \
    BRAYNS_JSON_ADAPTER_SET("material", setMaterial, "Material parameters")

BRAYNS_JSON_ADAPTER_BEGIN(ModelMaterial<CarPaintMaterial>)
MODEL_MATERIAL_PARAMS()
BRAYNS_JSON_ADAPTER_END()

BRAYNS_JSON_ADAPTER_BEGIN(ModelMaterial<DefaultMaterial>)
MODEL_MATERIAL_PARAMS()
BRAYNS_JSON_ADAPTER_END()

BRAYNS_JSON_ADAPTER_BEGIN(ModelMaterial<EmissiveMaterial>)
MODEL_MATERIAL_PARAMS()
BRAYNS_JSON_ADAPTER_END()

BRAYNS_JSON_ADAPTER_BEGIN(ModelMaterial<GlassMaterial>)
MODEL_MATERIAL_PARAMS()
BRAYNS_JSON_ADAPTER_END()

BRAYNS_JSON_ADAPTER_BEGIN(ModelMaterial<MatteMaterial>)
MODEL_MATERIAL_PARAMS()
BRAYNS_JSON_ADAPTER_END()

BRAYNS_JSON_ADAPTER_BEGIN(ModelMaterial<MetalMaterial>)
MODEL_MATERIAL_PARAMS()
BRAYNS_JSON_ADAPTER_END()

BRAYNS_JSON_ADAPTER_BEGIN(ModelMaterial<PlasticMaterial>)
MODEL_MATERIAL_PARAMS()
BRAYNS_JSON_ADAPTER_END()
}
