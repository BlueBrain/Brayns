/* Copyright (c) 2021 EPFL/Blue Brain Project
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

#include <brayns/network/common/ExtractMaterial.h>
#include <brayns/network/common/ExtractModel.h>
#include <brayns/network/json/MessageAdapter.h>

#include <common/commonTypes.h>
#include <common/types.h>

namespace brayns
{
class MaterialRef
{
public:
    bool hasMaterial() const { return _material; }

    Material& getMaterial() const { return *_material; }

    void setMaterial(Material& material) { _material = &material; }

private:
    Material* _material = nullptr;
};

class BaseMaterial : public MaterialRef
{
public:
    const Vector3d& getDiffuseColor() const
    {
        return hasMaterial() ? getMaterial().getDiffuseColor()
                             : _getDefaultVector();
    }

    void setDiffuseColor(const Vector3d& value) const
    {
        if (hasMaterial())
        {
            getMaterial().setDiffuseColor(value);
        }
    }

    const Vector3d& getSpecularColor() const
    {
        return hasMaterial() ? getMaterial().getSpecularColor()
                             : _getDefaultVector();
    }

    void setSpecularColor(const Vector3d& value) const
    {
        if (hasMaterial())
        {
            getMaterial().setSpecularColor(value);
        }
    }

    double getSpecularExponent() const
    {
        return hasMaterial() ? getMaterial().getSpecularExponent() : 0.0;
    }

    void setSpecularExponent(double value) const
    {
        if (hasMaterial())
        {
            getMaterial().setSpecularExponent(value);
        }
    }

    double getReflectionIndex() const
    {
        return hasMaterial() ? getMaterial().getReflectionIndex() : 0.0;
    }

    void setReflectionIndex(double value) const
    {
        if (hasMaterial())
        {
            getMaterial().setReflectionIndex(value);
        }
    }

    double getOpacity() const
    {
        return hasMaterial() ? getMaterial().getOpacity() : 0.0;
    }

    void setOpacity(double value) const
    {
        if (hasMaterial())
        {
            getMaterial().setOpacity(value);
        }
    }

    double getRefractionIndex() const
    {
        return hasMaterial() ? getMaterial().getRefractionIndex() : 0.0;
    }

    void setRefractionIndex(double value) const
    {
        if (hasMaterial())
        {
            getMaterial().setRefractionIndex(value);
        }
    }

    double getEmission() const
    {
        return hasMaterial() ? getMaterial().getEmission() : 0.0;
    }

    void setEmission(double value) const
    {
        if (hasMaterial())
        {
            getMaterial().setEmission(value);
        }
    }

    double getGlossiness() const
    {
        return hasMaterial() ? getMaterial().getGlossiness() : 0.0;
    }

    void setGlossiness(double value) const
    {
        if (hasMaterial())
        {
            getMaterial().setGlossiness(value);
        }
    }

private:
    static const Vector3d& _getDefaultVector()
    {
        static const Vector3d value{};
        return value;
    }
};

class ExtendedMaterial : public BaseMaterial
{
public:
    bool getSimulationDataCast() const
    {
        return _valueOr(MATERIAL_PROPERTY_CAST_USER_DATA, false);
    }

    void setSimulationDataCast(bool value) const
    {
        _setValue(MATERIAL_PROPERTY_CAST_USER_DATA, value);
    }

    MaterialShadingMode getShadingMode() const
    {
        auto code = _valueOr(MATERIAL_PROPERTY_SHADING_MODE, 0);
        return MaterialShadingMode(code);
    }

    void setShadingMode(MaterialShadingMode value) const
    {
        _setValue(MATERIAL_PROPERTY_SHADING_MODE, int(value));
    }

    MaterialClippingMode getClippingMode() const
    {
        auto code = _valueOr(MATERIAL_PROPERTY_CLIPPING_MODE, 0);
        return MaterialClippingMode(code);
    }

    void setClippingMode(MaterialClippingMode value) const
    {
        _setValue(MATERIAL_PROPERTY_CLIPPING_MODE, int(value));
    }

    double getUserParameter() const
    {
        return _valueOr(MATERIAL_PROPERTY_USER_PARAMETER, 0.0);
    }

    void setUserParameter(double value) const
    {
        _setValue(MATERIAL_PROPERTY_USER_PARAMETER, value);
    }

private:
    template <typename T>
    T _valueOr(const std::string& name, T defaultValue) const
    {
        if (!hasMaterial())
        {
            return defaultValue;
        }
        return getMaterial().getPropertyOrValue(name, std::move(defaultValue));
    }

    template <typename T>
    void _setValue(const std::string& name, T value) const
    {
        if (!hasMaterial())
        {
            return;
        }
        getMaterial().updateProperty(name, value);
    }
};

class MaterialProxy : public ExtendedMaterial
{
public:
    MaterialProxy() = default;

    MaterialProxy(Scene& scene)
        : _scene(&scene)
    {
    }

    size_t getModelId() const { return _model ? _model->getModelID() : 0; }

    void setModelId(size_t id) { _model = &ExtractModel::fromId(*_scene, id); }

    size_t getMaterialId() const { return _materialId; }

    void setMaterialId(size_t id)
    {
        if (!_model)
        {
            return;
        }
        _materialId = id;
        setMaterial(ExtractMaterial::fromId(*_model, id));
    }

    void commit()
    {
        if (hasMaterial())
        {
            getMaterial().commit();
        }
    }

private:
    Scene* _scene = nullptr;
    ModelDescriptor* _model = nullptr;
    size_t _materialId = 0;
};

BRAYNS_ADAPTER_ENUM(MaterialShadingMode, {"none", MaterialShadingMode::none},
                    {"diffuse", MaterialShadingMode::diffuse},
                    {"electron", MaterialShadingMode::electron},
                    {"cartoon", MaterialShadingMode::cartoon},
                    {"electron_transparency",
                     MaterialShadingMode::electron_transparency},
                    {"perlin", MaterialShadingMode::perlin},
                    {"diffuse_transparency",
                     MaterialShadingMode::diffuse_transparency},
                    {"checker", MaterialShadingMode::checker})

BRAYNS_ADAPTER_ENUM(MaterialClippingMode,
                    {"no_clipping", MaterialClippingMode::no_clipping},
                    {"plane", MaterialClippingMode::plane},
                    {"sphere", MaterialClippingMode::sphere})

#define BRAYNS_MATERIAL_PROPERTIES()                                           \
    BRAYNS_ADAPTER_GETSET("diffuse_color", getDiffuseColor, setDiffuseColor,   \
                          "Diffuse reflection color RGB normalized")           \
    BRAYNS_ADAPTER_GETSET("specular_color", getSpecularColor,                  \
                          setSpecularColor,                                    \
                          "Specular reflection RGB normalized")                \
    BRAYNS_ADAPTER_GETSET(                                                     \
        "specular_exponent", getSpecularExponent, setSpecularExponent,         \
        "The specular exponent to sharpen the specular reflection")            \
    BRAYNS_ADAPTER_GETSET("reflection_index", getReflectionIndex,              \
                          setReflectionIndex,                                  \
                          "The index of reflection of the material surface")   \
    BRAYNS_ADAPTER_GETSET("opacity", getOpacity, setOpacity,                   \
                          "The transparency of the material (0 to 1)")         \
    BRAYNS_ADAPTER_GETSET("refraction_index", getRefractionIndex,              \
                          setRefractionIndex,                                  \
                          "The index of refraction of a transparent material") \
    BRAYNS_ADAPTER_GETSET("emission", getEmission, setEmission,                \
                          "The emissive property of a material")               \
    BRAYNS_ADAPTER_GETSET("glossiness", getGlossiness, setGlossiness,          \
                          "The glossy component of a material")

#define BRAYNS_EXTENDED_MATERIAL_PROPERTIES()                                 \
    BRAYNS_ADAPTER_GETSET("simulation_data_cast", getSimulationDataCast,      \
                          setSimulationDataCast,                              \
                          "Wether to cast the user parameter for simulation") \
    BRAYNS_ADAPTER_GETSET("shading_mode", getShadingMode, setShadingMode,     \
                          "The chosen shading mode")                          \
    BRAYNS_ADAPTER_GETSET("clipping_mode", getClippingMode, setClippingMode,  \
                          "The choosen material clipping mode")               \
    BRAYNS_ADAPTER_GETSET("user_parameter", getUserParameter,                 \
                          setUserParameter,                                   \
                          "A custom parameter passed to the simulation")

BRAYNS_ADAPTER_BEGIN(BaseMaterial)
BRAYNS_MATERIAL_PROPERTIES()
BRAYNS_ADAPTER_END()

BRAYNS_ADAPTER_BEGIN(ExtendedMaterial)
BRAYNS_MATERIAL_PROPERTIES()
BRAYNS_EXTENDED_MATERIAL_PROPERTIES()
BRAYNS_ADAPTER_END()

BRAYNS_NAMED_ADAPTER_BEGIN(MaterialProxy, "Material")
BRAYNS_ADAPTER_GETSET("model_id", getModelId, setModelId,
                      "The model which this material belongs to")
BRAYNS_ADAPTER_GETSET("material_id", getMaterialId, setMaterialId,
                      "The ID that identifies this material")
BRAYNS_MATERIAL_PROPERTIES()
BRAYNS_EXTENDED_MATERIAL_PROPERTIES()
BRAYNS_ADAPTER_END()
} // namespace brayns