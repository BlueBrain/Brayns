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

namespace brayns
{
MaterialManager::MaterialManager()
{
}

MaterialManager::~MaterialManager()
{
}

void MaterialManager::clear()
{
    _materials.clear();
}

Material& MaterialManager::get(const size_t index)
{
    return _materials[_materialMapping[index]];
}

void MaterialManager::set(const size_t index, const Material& material)
{
    if (_materialMapping.find(index) == _materialMapping.end())
    {
        _materialMapping[index] = _materials.size();
        _materials.push_back(material);
    }
    else
    {
        const auto position = _materialMapping[index];
        _materials[position] = material;
    }
}

void MaterialManager::set(const size_t index)
{
    if (_materialMapping.find(index) == _materialMapping.end())
        set(index, Material());
}

size_t MaterialManager::position(const size_t materialId)
{
    const auto it = _materialMapping.find(materialId);
    const auto distance = std::distance(_materialMapping.begin(), it);
    return distance;
}

void MaterialManager::reset()
{
}
}
