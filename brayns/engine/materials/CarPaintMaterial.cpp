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

#include <brayns/engine/materials/CarPaintMaterial.h>

namespace brayns
{
std::string CarPaintMaterial::getName() const noexcept
{
    return "car paint";
}

uint64_t CarPaintMaterial::getSizeInBytes() const noexcept
{
    // We copy the data to OSPRay, so we must account for that
    return sizeof(CarPaintMaterial) * 2 - sizeof(OSPMaterial);
}

void CarPaintMaterial::setFlakesDesnity(const float flakeDensity) noexcept
{
    _updateValue(_flakeDensity, glm::clamp(flakeDensity, 0.f, 1.f));
}

float CarPaintMaterial::getFlakesDensity() const noexcept
{
    return _flakeDensity;
}

void CarPaintMaterial::commitMaterialSpecificParams()
{
    auto ospHandle = handle();

    ospSetParam(ospHandle, "baseColor", OSPDataType::OSP_VEC3F, &BASE_COLOR_WHITE);
    ospSetParam(ospHandle, "flakeDensity", OSPDataType::OSP_FLOAT, &_flakeDensity);
}

std::string_view CarPaintMaterial::getOSPHandleName() const noexcept
{
    return "carPaint";
}
}
