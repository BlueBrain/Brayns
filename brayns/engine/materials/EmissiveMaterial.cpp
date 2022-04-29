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

#include <brayns/engine/materials/EmissiveMaterial.h>

namespace brayns
{
EmissiveMaterial::EmissiveMaterial()
    : Material("luminous")
{
}

std::string EmissiveMaterial::getName() const noexcept
{
    return "emissive";
}

void EmissiveMaterial::setIntensity(const float intensity) noexcept
{
    _updateValue(_intensity, glm::max(intensity, 0.f));
}

float EmissiveMaterial::getIntensity() const noexcept
{
    return _intensity;
}

void EmissiveMaterial::commitMaterialSpecificParams()
{
    auto ospHandle = handle();

    ospSetParam(ospHandle, "color", OSPDataType::OSP_VEC3F, &getColor());
    ospSetParam(ospHandle, "intensity", OSPDataType::OSP_FLOAT, &_intensity);
}
}
