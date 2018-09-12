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

#include "DirectionalLight.h"

namespace bbp
{
namespace optix
{
void DirectionalLight::commit()
{
    const ospray::vec3f direction =
        getParam3f("direction", ospray::vec3f(0.f, 0.f, 1.f));
    optixLight.pos.x = direction.x;
    optixLight.pos.y = direction.y;
    optixLight.pos.z = direction.z;
    Light::commit();
}
}
}
