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

#include <brayns/common/ImageManager.h>
#include <brayns/common/log.h>

namespace brayns
{
Material::Material(const PropertyMap& properties)
{
    setCurrentType("default");
    _properties.at(_currentType).merge(properties);
}

Texture2DPtr Material::getTexture(const TextureType type) const
{
    const auto it = _textureDescriptors.find(type);
    if (it == _textureDescriptors.end())
        throw std::runtime_error("Failed to get texture with type " +
                                 std::to_string(static_cast<int>(type)));
    return it->second;
}

void Material::clearTextures()
{
    _textureDescriptors.clear();
    markModified();
}

bool Material::_loadTexture(const std::string& fileName, const TextureType type)
{
    if (_textures.find(fileName) != _textures.end())
        return true;

    auto texture = ImageManager::importTextureFromFile(fileName, type);
    if (!texture)
        return false;

    _textures[fileName] = texture;
    BRAYNS_DEBUG << fileName << ": " << texture->width << "x" << texture->height
                 << "x" << (int)texture->channels << "x" << (int)texture->depth
                 << " added to the texture cache" << std::endl;
    return true;
}

void Material::setTexture(const std::string& fileName, const TextureType type)
{
    auto i = _textureDescriptors.find(type);
    if (i != _textureDescriptors.end() && i->second->filename == fileName)
        return;

    if (_textures.find(fileName) == _textures.end())
        if (!_loadTexture(fileName, type))
            throw std::runtime_error("Failed to load texture from " + fileName);
    _textureDescriptors[type] = _textures[fileName];
    markModified();
}

void Material::removeTexture(const TextureType type)
{
    auto i = _textureDescriptors.find(type);
    if (i == _textureDescriptors.end())
        return;

    _textureDescriptors.erase(i);
    markModified();
}
} // namespace brayns
