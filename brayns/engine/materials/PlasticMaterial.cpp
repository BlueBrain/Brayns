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

#include "PlasticMaterial.h"

#include <brayns/engine/common/MathTypesOsprayTraits.h>

namespace
{
struct PlasticParameters
{
    inline static const std::string osprayName = "principled";
    inline static const std::string color = "baseColor";
    inline static const std::string roughness = "roughness";
    inline static const std::string coat = "coat";
    inline static const std::string coatThickness = "coatThickness";
    inline static const std::string sheen = "sheen";
    inline static const std::string opacity = "opacity";
};
}

namespace brayns
{
PlasticMaterial::PlasticMaterial()
    : Material(PlasticParameters::osprayName)
{
}

std::string PlasticMaterial::getName() const noexcept
{
    return "plastic";
}

void PlasticMaterial::setOpacity(const float opacity) noexcept
{
    _updateValue(_opacity, glm::clamp(opacity, 0.f, 1.f));
}

float PlasticMaterial::getOpacity() const noexcept
{
    return _opacity;
}

void PlasticMaterial::commitMaterialSpecificParams()
{
    constexpr float clearCoat = 1.f;
    constexpr float clearCoatThickness = 3.f;
    constexpr float roughness = 0.01f;
    constexpr float sheen = 1.f;
    const auto overridedColorWhite = brayns::Vector3f(1.f);

    auto &osprayMaterial = getOsprayMaterial();
    osprayMaterial.setParam(PlasticParameters::color, overridedColorWhite);
    osprayMaterial.setParam(PlasticParameters::roughness, roughness);
    osprayMaterial.setParam(PlasticParameters::coat, clearCoat);
    osprayMaterial.setParam(PlasticParameters::coatThickness, clearCoatThickness);
    osprayMaterial.setParam(PlasticParameters::sheen, sheen);
    osprayMaterial.setParam(PlasticParameters::opacity, _opacity);
}
}
