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

#include <brayns/engine/materials/MetalMaterial.h>

namespace brayns
{
std::string MetalMaterial::getName() const noexcept
{
    return "metal";
}

uint64_t MetalMaterial::getSizeInBytes() const noexcept
{
    // We copy all data to ospray, so we must account for that
    return sizeof(MetalMaterial) * 2 - sizeof(OSPMaterial);
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
    auto ospHandle = handle();
    ospSetParam(ospHandle, "color", OSPDataType::OSP_VEC3F, &BASE_COLOR_WHITE);
    ospSetParam(ospHandle, "roughness", OSPDataType::OSP_FLOAT, &_roughness);
}

std::string MetalMaterial::getOSPHandleName() const noexcept
{
    return "alloy";
}
}
