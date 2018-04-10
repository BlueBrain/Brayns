/* Copyright (c) 2015-2017, EPFL/Blue Brain Project
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

#include "MaterialManager.h"

#include <brayns/common/log.h>
#include <brayns/common/material/Material.h>
#include <brayns/io/ImageManager.h>
#include <brayns/parameters/ParametersManager.h>

namespace brayns
{
MaterialManager::MaterialManager(ParametersManager& parametersManager)
    : _parametersManager(parametersManager)
{
    clear();
}

MaterialManager::~MaterialManager()
{
}

void MaterialManager::clear()
{
    _materials.clear();
    _initializeSystemMaterials();
}

void MaterialManager::check(const size_t index)
{
    if (index > _materials.size())
        for (size_t i = _materials.size(); i <= index; ++i)
            add(Material());
}

Material& MaterialManager::get(const size_t index)
{
    check(index);
    return _materials[index];
}

void MaterialManager::set(const size_t index, Material material)
{
    check(index);
    const auto it = _materials.begin() + index;
    _materials.erase(it);
    _materials.insert(it, material);
}

size_t MaterialManager::add(const Material& material)
{
    _materials.push_back(material);
    return _materials.size() - 1;
}

void MaterialManager::remove(const size_t index)
{
    if (index < _materials.size())
        _materials.erase(_materials.begin() + index);
}

size_t MaterialManager::addTexture(const std::string& filename)
{
    // Check if texture is already loaded
    size_t index = 0;
    bool found = false;
    for (auto texture : _textures)
    {
        if (texture.second->getFilename() == filename)
        {
            found = true;
            break;
        }
        ++index;
    }
    if (!found)
    {
        BRAYNS_INFO << "Loading texture from " << filename << std::endl;
        ImageManager::importTextureFromFile(_textures, index, filename);
    }
    return index;
}

void MaterialManager::_initializeSystemMaterials()
{
    BRAYNS_FCT_ENTRY

    Material material;
    material.setDiffuseColor(Vector3f(1.f, 1.f, 1.f));
    material.setEmission(10.f);
    material.setName("scene_skybox");

    // set environment map if applicable
    const auto& environmentMap =
        _parametersManager.getSceneParameters().getEnvironmentMap();
    if (!environmentMap.empty())
        material.addTexture(TT_DIFFUSE, 0);

    add(material);
}
}
