/* Copyright (c) 2017, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Daniel Nachbaur <daniel.nachbaur@epfl.ch>
 *
 * This file is part of https://github.com/BlueBrain/ospray-modules
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

#include "Light.h"

#include "../Context.h"

namespace bbp
{
namespace optix
{
void Light::commit()
{
    const ospray::vec3f color = getParam3f("color", ospray::vec3f(1.f));
    float angularDiameter = getParam1f("angularDiameter", .0f);

    angularDiameter = ospray::clamp(angularDiameter, 0.f, 180.f);

    optixLight.color.x = color.x;
    optixLight.color.y = color.y;
    optixLight.color.z = color.z;

    optixLight.casts_shadow = 1;
    optixLight.type = static_cast<int>(type);

    Context::get().updateLights();
}
}
}
