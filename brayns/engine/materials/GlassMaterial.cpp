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

#include <brayns/engine/materials/GlassMaterial.h>

namespace brayns
{
std::string GlassMaterial::getName() const noexcept
{
    return "glass";
}

uint64_t GlassMaterial::getSizeInBytes() const noexcept
{
    // We copy all data to ospray, so we must account for that
    return sizeof(GlassMaterial) * 2 - sizeof(OSPMaterial);
}

void GlassMaterial::setColor(const Vector3f &color) noexcept
{
    _updateValue(_color, glm::clamp(color, Vector3f(0.f), Vector3f(1.f)));
}

void GlassMaterial::setIndexOfRefraction(const float ior) noexcept
{
    _updateValue(_ior, ior);
}

const Vector3f &GlassMaterial::getColor() const noexcept
{
    return _color;
}

float GlassMaterial::getIndexOfRefraction() const noexcept
{
    return _ior;
}

void GlassMaterial::commitMaterialSpecificParams()
{
    auto ospHandle = handle();

    ospSetParam(ospHandle, "attenuationColor", OSPDataType::OSP_VEC3F, &_color);
    ospSetParam(ospHandle, "eta", OSPDataType::OSP_FLOAT, &_ior);
}

std::string_view GlassMaterial::getOSPHandleName() const noexcept
{
    return "glass";
}
}
