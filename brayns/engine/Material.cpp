/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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
Material::Material(const std::string &handleID)
    : _osprayMaterial("", handleID)
{
}

bool Material::commit()
{
    if (!_flag)
    {
        return false;
    }

    commitMaterialSpecificParams();

    _osprayMaterial.commit();

    _flag = false;

    return true;
}

const ospray::cpp::Material &Material::getOsprayMaterial() const noexcept
{
    return _osprayMaterial;
}

void Material::setColor(const Vector3f &color) noexcept
{
    _flag.update(_color, color);
}

const Vector3f &Material::getColor() const noexcept
{
    return _color;
}

ModifiedFlag &Material::getModifiedFlag() noexcept
{
    return _flag;
}

} // namespace brayns
