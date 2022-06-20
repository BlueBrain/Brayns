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

#include "MetalMaterial.h"

#include <brayns/engine/ospray/OsprayMathtypesTraits.h>

namespace
{
class MetalParameterUpdater
{
public:
    static void update(const brayns::MetalMaterial &material)
    {
        static const std::string colorParameter = "baseColor";
        static const std::string roughnessParameter = "roughness";

        const auto overridedColorWhite = brayns::Vector3f(1.f);
        auto roughness = material.getRoughness();

        const auto &osprayMaterial = material.getOsprayMaterial();
        osprayMaterial.setParam(colorParameter, overridedColorWhite);
        osprayMaterial.setParam(roughnessParameter, roughness);
    }
};
}

namespace brayns
{
MetalMaterial::MetalMaterial()
    : Material("alloy")
{
}

std::string MetalMaterial::getName() const noexcept
{
    return "metal";
}

void MetalMaterial::setRoughness(const float roughness) noexcept
{
    _updateValue(_roughness, glm::clamp(roughness, 0.f, 1.f));
}

float MetalMaterial::getRoughness() const noexcept
{
    return _roughness;
}

void MetalMaterial::commitMaterialSpecificParams()
{
    MetalParameterUpdater::update(*this);
}
}
