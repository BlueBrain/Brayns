/* Copyright (c) 2015-2021, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
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

#include "Material.h"

namespace brayns
{
Material::Material(const PropertyMap &properties)
{
    setCurrentType("default");
    _properties.at(_currentType).merge(properties);
}

const std::string &Material::getName() const
{
    return _name;
}

void Material::setName(const std::string &value)
{
    _updateValue(_name, value);
}

void Material::setDiffuseColor(const Vector3d &value)
{
    _updateValue(_diffuseColor, value);
}

const Vector3d &Material::getDiffuseColor() const
{
    return _diffuseColor;
}

void Material::setSpecularColor(const Vector3d &value)
{
    _updateValue(_specularColor, value);
}

const Vector3d &Material::getSpecularColor() const
{
    return _specularColor;
}

void Material::setSpecularExponent(double value)
{
    _updateValue(_specularExponent, value);
}

double Material::getSpecularExponent() const
{
    return _specularExponent;
}

void Material::setReflectionIndex(double value)
{
    _updateValue(_reflectionIndex, value);
}

double Material::getReflectionIndex() const
{
    return _reflectionIndex;
}

void Material::setOpacity(double value)
{
    _updateValue(_opacity, value);
}

double Material::getOpacity() const
{
    return _opacity;
}

void Material::setRefractionIndex(double value)
{
    _updateValue(_refractionIndex, value);
}

double Material::getRefractionIndex() const
{
    return _refractionIndex;
}

void Material::setEmission(double value)
{
    _updateValue(_emission, value);
}

double Material::getEmission() const
{
    return _emission;
}

void Material::setGlossiness(double value)
{
    _updateValue(_glossiness, value);
}

double Material::getGlossiness() const
{
    return _glossiness;
}
} // namespace brayns
