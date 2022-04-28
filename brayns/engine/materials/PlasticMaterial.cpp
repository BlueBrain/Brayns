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

#include <brayns/engine/materials/PlasticMaterial.h>

namespace brayns
{
PlasticMaterial::PlasticMaterial()
    : Material("principled")
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

    auto ospHandle = handle();

    ospSetParam(ospHandle, "baseColor", OSPDataType::OSP_VEC3F, &overridedColorWhite);
    ospSetParam(ospHandle, "roughness", OSPDataType::OSP_FLOAT, &roughness);
    ospSetParam(ospHandle, "coat", OSPDataType::OSP_FLOAT, &clearCoat);
    ospSetParam(ospHandle, "coatThickness", OSPDataType::OSP_FLOAT, &clearCoatThickness);
    ospSetParam(ospHandle, "sheen", OSPDataType::OSP_FLOAT, &sheen);
    ospSetParam(ospHandle, "opacity", OSPDataType::OSP_FLOAT, &_opacity);
}
}
