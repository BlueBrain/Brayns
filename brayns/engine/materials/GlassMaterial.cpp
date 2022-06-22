/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include "GlassMaterial.h"

#include <brayns/engine/common/MathTypesOsprayTraits.h>

namespace
{
struct GlassParameters
{
    inline static const std::string osprayName = "thinGlass";
    inline static const std::string attenuation = "attenuationColor";
    inline static const std::string ior = "eta";
};
}

namespace brayns
{
GlassMaterial::GlassMaterial()
    : Material(GlassParameters::osprayName)
{
}

std::string GlassMaterial::getName() const noexcept
{
    return "glass";
}

void GlassMaterial::setIndexOfRefraction(const float ior) noexcept
{
    _updateValue(_ior, ior);
}

float GlassMaterial::getIndexOfRefraction() const noexcept
{
    return _ior;
}

void GlassMaterial::commitMaterialSpecificParams()
{
    const auto overridedColorWhite = brayns::Vector3f(1.f);

    auto &osprayMaterial = getOsprayMaterial();
    osprayMaterial.setParam(GlassParameters::attenuation, overridedColorWhite);
    osprayMaterial.setParam(GlassParameters::ior, _ior);
}
}
