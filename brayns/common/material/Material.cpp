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

#include <brayns/io/ImageManager.h>

namespace brayns
{
Material::Material()
    : BaseObject()
{
}

Texture2DPtr Material::getTexture(const TextureType& type)
{
    if (_textureDescriptors.find(type) == _textureDescriptors.end())
        return nullptr;
    return _textureDescriptors[type];
}

bool Material::_loadTexture(const std::string& fileName)
{
    if (_textures.find(fileName) != _textures.end())
        return true;

    auto texture = ImageManager::importTextureFromFile(fileName);
    if (!texture)
        return false;

    _textures[fileName] = texture;
    return true;
}

void Material::setTexture(const std::string& fileName, const TextureType& type)
{
    if (_textures.find(fileName) == _textures.end())
        if (!_loadTexture(fileName))
            throw std::runtime_error("Failed to load texture from " + fileName);
    _textureDescriptors[type] = _textures[fileName];
    markModified();
}
}
