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

#include <brayns/engine/materials/MatteMaterial.h>

namespace brayns
{
std::string MatteMaterial::getName() const noexcept
{
    return "matte";
}

void MatteMaterial::setOpacity(const float opacity) noexcept
{
    _updateValue(_opacity, glm::clamp(opacity, 0.f, 1.f));
}

float MatteMaterial::getOpacity() const noexcept
{
    return _opacity;
}

void MatteMaterial::commitMaterialSpecificParams()
{
    static constexpr float roughness = 1.f;

    auto ospHandle = handle();

    ospSetParam(ospHandle, "baseColor", OSPDataType::OSP_VEC3F, &BASE_COLOR_WHITE);
    ospSetParam(ospHandle, "roughness", OSPDataType::OSP_FLOAT, &roughness);
    ospSetParam(ospHandle, "opacity", OSPDataType::OSP_FLOAT, &_opacity);
}

std::string MatteMaterial::getOSPHandleName() const noexcept
{
    return "principled";
}
}
