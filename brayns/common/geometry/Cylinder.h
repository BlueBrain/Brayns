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
struct Cylinder
{
#if __cplusplus
    Cylinder(const Vector3f c = {0.f, 0.f, 0.f},
             const Vector3f u = {0.f, 0.f, 0.f}, const float r = 0.f,
             const float ts = 0.f, const Vector2f t = Vector2f(), int32 m = -1)
        : center(c)
        , up(u)
        , radius(r)
        , timestamp(ts)
        , texture_coords(t)
        , materialID(m)
    {
    }
#endif

    VEC3_TYPE center;
    VEC3_TYPE up;
    float radius;
    float timestamp;
    VEC2_TYPE texture_coords;
    int32 materialID;
};

#if __cplusplus
} // brayns
#endif
