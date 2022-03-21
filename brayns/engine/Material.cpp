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

#include <brayns/engine/Material.h>

namespace brayns
{

const Vector3f Material::BASE_COLOR_WHITE = Vector3f(1.f);

Material::Material(const Material& o)
{
    (void)o;
}

Material &Material::operator=(const Material &o)
{
    (void)o;
    markModified();
    return *this;
}

Material::~Material()
{
    if(_handle)
        ospRelease(_handle);
}

bool Material::commit()
{
    if(!isModified())
    {
        return false;
    }

    if(!_handle)
    {
        const auto handleName = getOSPHandleName();
        _handle = ospNewMaterial("", handleName.data());
    }

    commitMaterialSpecificParams();

    ospCommit(_handle);

    resetModified();

    return true;
}

OSPMaterial Material::handle() const noexcept
{
    return _handle;
}

void Material::setColor(const Vector3f &color) noexcept
{
    _updateValue(_color, glm::clamp(color, Vector3f(0.f), Vector3f(1.f)));
}

const Vector3f &Material::getColor() const noexcept
{
    return _color;
}

} // namespace brayns
