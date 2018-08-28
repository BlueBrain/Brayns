/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
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

#pragma once

#include "CommonDefines.h"

#if __cplusplus
namespace brayns
{
#endif

struct Sphere
{
#if __cplusplus
    Sphere(const Vector3f c = {0.f, 0.f, 0.f}, float r = 0.f, float ts = 0.f,
           const Vector2f t = {0.f, 0.f})
        : center(c)
        , radius(r)
        , timestamp(ts)
        , texture_coords(t)
    {
    }
#endif

    VEC3_TYPE center;
    float radius;
    float timestamp;
    VEC2_TYPE texture_coords;
};

#if __cplusplus
} // brayns
#endif
