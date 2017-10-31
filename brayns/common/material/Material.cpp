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
Material::Material()
    : _color(1.f, 1.f, 1.f)
    , _specularColor(1.f, 1.f, 1.f)
    , _specularExponent(10.f)
    , _reflectionIndex(0.f)
    , _opacity(1.f)
    , _refractionIndex(1.f)
    , _emission(0.f)
    , _glossiness(0.f)
    , _locked(false)
{
}

void Material::setTexture(const TextureType& type, const std::string& filename)
{
    _textures[type] = filename;
}
}
