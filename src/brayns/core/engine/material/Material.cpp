/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *                     Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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
Material::Material(Material &&other) noexcept
{
    *this = std::move(other);
}

Material &Material::operator=(Material &&other) noexcept
{
    _handleName = std::move(other._handleName);
    _materialName = std::move(other._materialName);
    _handle = std::move(other._handle);
    _data = std::move(other._data);
    _flag = std::move(other._flag);
    return *this;
}

Material::Material(const Material &other)
{
    *this = other;
}

Material &Material::operator=(const Material &other)
{
    _handleName = other._handleName;
    _materialName = other._materialName;
    _handle = ospray::cpp::Material(_handleName);
    _data = other._data->clone();
    _data->pushTo(_handle);
    _flag.setModified(true);
    return *this;
}

bool Material::commit()
{
    if (!_flag)
    {
        return false;
    }
    _flag = false;
    _handle.commit();
    return true;
}

std::string Material::getName() const noexcept
{
    return _materialName;
}

const ospray::cpp::Material &Material::getHandle() const noexcept
{
    return _handle;
}
}
