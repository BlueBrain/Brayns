/* Copyright (c) 2015-2022 EPFL/Blue Brain Project
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

#include "MaterialAdapter.h"

#include <brayns/network/common/ExtractMaterial.h>
#include <brayns/network/common/ExtractModel.h>

#include <plugin/api/MaterialUtils.h>

namespace brayns
{
MaterialRef::MaterialRef(Material &material)
    : _material(&material)
{
}

bool MaterialRef::hasMaterial() const
{
    return _material;
}

Material &MaterialRef::getMaterial() const
{
    return *_material;
}

void MaterialRef::setMaterial(Material &material)
{
    _material = &material;
}

void MaterialRef::commit()
{
    if (_material)
    {
        _material->commit();
    }
}

BaseMaterial::BaseMaterial(Material &material)
    : MaterialRef(material)
{
}

const Vector3d &BaseMaterial::getDiffuseColor() const
{
    return hasMaterial() ? getMaterial().getDiffuseColor() : _getDefaultVector();
}

void BaseMaterial::setDiffuseColor(const Vector3d &value) const
{
    if (hasMaterial())
    {
        getMaterial().setDiffuseColor(value);
    }
}

const Vector3d &BaseMaterial::getSpecularColor() const
{
    return hasMaterial() ? getMaterial().getSpecularColor() : _getDefaultVector();
}

void BaseMaterial::setSpecularColor(const Vector3d &value) const
{
    if (hasMaterial())
    {
        getMaterial().setSpecularColor(value);
    }
}

double BaseMaterial::getSpecularExponent() const
{
    return hasMaterial() ? getMaterial().getSpecularExponent() : 0.0;
}

void BaseMaterial::setSpecularExponent(double value) const
{
    if (hasMaterial())
    {
        getMaterial().setSpecularExponent(value);
    }
}

double BaseMaterial::getReflectionIndex() const
{
    return hasMaterial() ? getMaterial().getReflectionIndex() : 0.0;
}

void BaseMaterial::setReflectionIndex(double value) const
{
    if (hasMaterial())
    {
        getMaterial().setReflectionIndex(value);
    }
}

double BaseMaterial::getOpacity() const
{
    return hasMaterial() ? getMaterial().getOpacity() : 0.0;
}

void BaseMaterial::setOpacity(double value) const
{
    if (hasMaterial())
    {
        getMaterial().setOpacity(value);
    }
}

double BaseMaterial::getRefractionIndex() const
{
    return hasMaterial() ? getMaterial().getRefractionIndex() : 0.0;
}

void BaseMaterial::setRefractionIndex(double value) const
{
    if (hasMaterial())
    {
        getMaterial().setRefractionIndex(value);
    }
}

double BaseMaterial::getEmission() const
{
    return hasMaterial() ? getMaterial().getEmission() : 0.0;
}

void BaseMaterial::setEmission(double value) const
{
    if (hasMaterial())
    {
        getMaterial().setEmission(value);
    }
}

double BaseMaterial::getGlossiness() const
{
    return hasMaterial() ? getMaterial().getGlossiness() : 0.0;
}

void BaseMaterial::setGlossiness(double value) const
{
    if (hasMaterial())
    {
        getMaterial().setGlossiness(value);
    }
}

const Vector3d &BaseMaterial::_getDefaultVector()
{
    static const Vector3d value{};
    return value;
}

ExtendedMaterial::ExtendedMaterial(Material &material)
    : BaseMaterial(material)
{
}

bool ExtendedMaterial::getSimulationDataCast() const
{
    return _valueOr(std::string(MATERIAL_PROPERTY_CAST_USER_DATA), false);
}

void ExtendedMaterial::setSimulationDataCast(bool value) const
{
    _setValue(std::string(MATERIAL_PROPERTY_CAST_USER_DATA), value);
}

MaterialShadingMode ExtendedMaterial::getShadingMode() const
{
    auto code = _valueOr(std::string(MATERIAL_PROPERTY_SHADING_MODE), 0);
    return MaterialShadingMode(code);
}

void ExtendedMaterial::setShadingMode(MaterialShadingMode value) const
{
    _setValue(std::string(MATERIAL_PROPERTY_SHADING_MODE), int(value));
}

MaterialClippingMode ExtendedMaterial::getClippingMode() const
{
    auto code = _valueOr(std::string(MATERIAL_PROPERTY_CLIPPING_MODE), 0);
    return MaterialClippingMode(code);
}

void ExtendedMaterial::setClippingMode(MaterialClippingMode value) const
{
    _setValue(std::string(MATERIAL_PROPERTY_CLIPPING_MODE), int(value));
}

double ExtendedMaterial::getUserParameter() const
{
    return _valueOr(std::string(MATERIAL_PROPERTY_USER_PARAMETER), 0.0);
}

void ExtendedMaterial::setUserParameter(double value) const
{
    _setValue(std::string(MATERIAL_PROPERTY_USER_PARAMETER), value);
}

MaterialProxy::MaterialProxy(Scene &scene)
    : _scene(&scene)
{
}

size_t MaterialProxy::getModelId() const
{
    return _model ? _model->getModelID() : 0;
}

void MaterialProxy::setModelId(size_t id)
{
    if (!_scene)
    {
        return;
    }
    _model = &ExtractModel::fromId(*_scene, id);
}

size_t MaterialProxy::getMaterialId() const
{
    return _materialId;
}

void MaterialProxy::setMaterialId(size_t id)
{
    if (!_model)
    {
        return;
    }
    _materialId = id;
    setMaterial(ExtractMaterial::fromId(*_model, id));
}
} // namespace brayns
