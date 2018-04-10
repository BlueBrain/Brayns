/* Copyright (c) 2015-2016, EPFL/Blue Brain Project
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
TextureDescriptor::TextureDescriptor()
{
}

TextureDescriptor::TextureDescriptor(const TextureType type, const size_t id)
    : _type(type)
    , _id(id)
{
}

TextureDescriptor::TextureDescriptor(const TextureDescriptor& rhs)
{
    this->_type = rhs._type;
    this->_id = rhs._id;
}

TextureDescriptor& TextureDescriptor::operator=(const TextureDescriptor& rhs)
{
    this->_type = rhs._type;
    this->_id = rhs._id;
    return *this;
}

Material::Material()
    : BaseObject()
{
}

Material::Material(const Material& rhs)
{
    this->_color = rhs._color;
    this->_name = rhs._name;
    this->_color = rhs._color;
    this->_specularColor = rhs._specularColor;
    this->_specularExponent = rhs._specularExponent;
    this->_reflectionIndex = rhs._reflectionIndex;
    this->_opacity = rhs._opacity;
    this->_refractionIndex = rhs._refractionIndex;
    this->_emission = rhs._emission;
    this->_glossiness = rhs._glossiness;
    this->_castSimulationData = rhs._castSimulationData;
    this->_textureDescriptors = rhs._textureDescriptors;
}

Material& Material::operator=(const Material& rhs)
{
    if (this == &rhs)
        return *this;

    this->_color = rhs._color;
    this->_name = rhs._name;
    this->_color = rhs._color;
    this->_specularColor = rhs._specularColor;
    this->_specularExponent = rhs._specularExponent;
    this->_reflectionIndex = rhs._reflectionIndex;
    this->_opacity = rhs._opacity;
    this->_refractionIndex = rhs._refractionIndex;
    this->_emission = rhs._emission;
    this->_glossiness = rhs._glossiness;
    this->_castSimulationData = rhs._castSimulationData;
    this->_textureDescriptors = rhs._textureDescriptors;
    return *this;
}

void Material::addTexture(const TextureType& type, const size_t id)
{
    TextureDescriptor textureDescriptor{type, id};
    _textureDescriptors.push_back(textureDescriptor);
    markModified();
}
}
