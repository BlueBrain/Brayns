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

#include <brayns/engine/lights/AmbientLight.h>

namespace brayns
{
std::string_view AmbientLight::getName() const noexcept
{
    return EngineObjectName<AmbientLight>::get();
}

std::string_view AmbientLight::getOSPHandleName() const noexcept
{
    return "ambient";
}

uint64_t AmbientLight::getSizeInBytes() const noexcept
{
    // We copy all the light data to OSPRay, so we must account for it
    return sizeof(AmbientLight) * 2 - sizeof(OSPLight);
}

void AmbientLight::commitLightSpecificParams()
{
}

template<>
std::string_view EngineObjectName<AmbientLight>::get()
{
    return "ambient";
}
}
