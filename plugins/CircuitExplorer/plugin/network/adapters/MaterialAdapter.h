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

#pragma once

#include <brayns/engine/Material.h>
#include <brayns/engine/Model.h>
#include <brayns/engine/Scene.h>
#include <brayns/json/JsonAdapterMacro.h>
#include <brayns/json/JsonBuffer.h>

#include <plugin/api/MaterialUtils.h>

namespace brayns
{
class MaterialRef
{
public:
    MaterialRef() = default;
    MaterialRef(Material &material);

    bool hasMaterial() const;
    Material &getMaterial() const;
    void setMaterial(Material &material);
    void commit();

private:
    Material *_material = nullptr;
};

class BaseMaterial : public MaterialRef
{
public:
    BaseMaterial() = default;
    BaseMaterial(Material &material);

    const Vector3d &getDiffuseColor() const;
    void setDiffuseColor(const Vector3d &value) const;
    const Vector3d &getSpecularColor() const;
    void setSpecularColor(const Vector3d &value) const;
    double getSpecularExponent() const;
    void setSpecularExponent(double value) const;
    double getReflectionIndex() const;
    void setReflectionIndex(double value) const;
    double getOpacity() const;
    void setOpacity(double value) const;
    double getRefractionIndex() const;
    void setRefractionIndex(double value) const;
    double getEmission() const;
    void setEmission(double value) const;
    double getGlossiness() const;
    void setGlossiness(double value) const;

private:
    static const Vector3d &_getDefaultVector();
};

class ExtendedMaterial : public BaseMaterial
{
public:
    ExtendedMaterial() = default;
    ExtendedMaterial(Material &material);

    bool getSimulationDataCast() const;
    void setSimulationDataCast(bool value) const;
    MaterialShadingMode getShadingMode() const;
    void setShadingMode(MaterialShadingMode value) const;
    MaterialClippingMode getClippingMode() const;
    void setClippingMode(MaterialClippingMode value) const;
    double getUserParameter() const;
    void setUserParameter(double value) const;

private:
    template<typename T>
    T _valueOr(const std::string &name, T defaultValue) const
    {
        if (!hasMaterial())
        {
            return defaultValue;
        }
        return getMaterial().getPropertyOrValue(name, std::move(defaultValue));
    }

    template<typename T>
    void _setValue(const std::string &name, T value) const
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
    MaterialProxy(Scene &scene);

    size_t getModelId() const;
    void setModelId(size_t id);
    size_t getMaterialId() const;
    void setMaterialId(size_t id);

private:
    Scene *_scene = nullptr;
    ModelDescriptor *_model = nullptr;
    size_t _materialId = 0;
};

BRAYNS_JSON_ADAPTER_ENUM(
    MaterialShadingMode,
    {"None", MaterialShadingMode::none},
    {"Diffuse", MaterialShadingMode::diffuse},
    {"Electron", MaterialShadingMode::electron},
    {"Cartoon", MaterialShadingMode::cartoon},
    {"Electron transparency", MaterialShadingMode::electron_transparency},
    {"Perlin", MaterialShadingMode::perlin},
    {"Diffuse transparency", MaterialShadingMode::diffuse_transparency},
    {"Checker", MaterialShadingMode::checker})

BRAYNS_JSON_ADAPTER_ENUM(
    MaterialClippingMode,
    {"No clipping", MaterialClippingMode::no_clipping},
    {"Plane", MaterialClippingMode::plane},
    {"Sphere", MaterialClippingMode::sphere})

#define BRAYNS_MATERIAL_PROPERTIES() \
    BRAYNS_JSON_ADAPTER_GETSET( \
        "diffuse_color", \
        getDiffuseColor, \
        setDiffuseColor, \
        "Diffuse reflection color RGB normalized") \
    BRAYNS_JSON_ADAPTER_GETSET( \
        "specular_color", \
        getSpecularColor, \
        setSpecularColor, \
        "Specular reflection RGB normalized") \
    BRAYNS_JSON_ADAPTER_GETSET( \
        "specular_exponent", \
        getSpecularExponent, \
        setSpecularExponent, \
        "The specular exponent to sharpen the specular reflection") \
    BRAYNS_JSON_ADAPTER_GETSET( \
        "reflection_index", \
        getReflectionIndex, \
        setReflectionIndex, \
        "The index of reflection of the material surface") \
    BRAYNS_JSON_ADAPTER_GETSET("opacity", getOpacity, setOpacity, "The transparency of the material (0 to 1)") \
    BRAYNS_JSON_ADAPTER_GETSET( \
        "refraction_index", \
        getRefractionIndex, \
        setRefractionIndex, \
        "The index of refraction of a transparent material") \
    BRAYNS_JSON_ADAPTER_GETSET("emission", getEmission, setEmission, "The emissive property of a material") \
    BRAYNS_JSON_ADAPTER_GETSET("glossiness", getGlossiness, setGlossiness, "The glossy component of a material")

#define BRAYNS_EXTENDED_MATERIAL_PROPERTIES() \
    BRAYNS_JSON_ADAPTER_GETSET( \
        "simulation_data_cast", \
        getSimulationDataCast, \
        setSimulationDataCast, \
        "Wether to cast the user parameter for simulation") \
    BRAYNS_JSON_ADAPTER_GETSET("shading_mode", getShadingMode, setShadingMode, "The chosen shading mode") \
    BRAYNS_JSON_ADAPTER_GETSET( \
        "clipping_mode", \
        getClippingMode, \
        setClippingMode, \
        "The choosen material clipping mode") \
    BRAYNS_JSON_ADAPTER_GETSET( \
        "user_parameter", \
        getUserParameter, \
        setUserParameter, \
        "A custom parameter passed to the simulation")

BRAYNS_JSON_ADAPTER_BEGIN(BaseMaterial)
BRAYNS_MATERIAL_PROPERTIES()
BRAYNS_JSON_ADAPTER_END()

BRAYNS_JSON_ADAPTER_BEGIN(ExtendedMaterial)
BRAYNS_MATERIAL_PROPERTIES()
BRAYNS_EXTENDED_MATERIAL_PROPERTIES()
BRAYNS_JSON_ADAPTER_END()

BRAYNS_NAMED_JSON_ADAPTER_BEGIN(MaterialProxy, "Material")
BRAYNS_JSON_ADAPTER_GETSET("model_id", getModelId, setModelId, "The model which this material belongs to")
BRAYNS_JSON_ADAPTER_GETSET("material_id", getMaterialId, setMaterialId, "The ID that identifies this material")
BRAYNS_MATERIAL_PROPERTIES()
BRAYNS_EXTENDED_MATERIAL_PROPERTIES()
BRAYNS_JSON_ADAPTER_END()
} // namespace brayns
