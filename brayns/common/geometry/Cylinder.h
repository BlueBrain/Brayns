/* Copyright (c) 2015-2017, EPFL/Blue Brain Project
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

#ifndef CYLINDER_H
#define CYLINDER_H

#include <brayns/common/types.h>

namespace brayns
{
struct Cylinder
{
    Cylinder(const Vector3f c, const Vector3f u, const float r,
             const float ts = 0.f, const Vector2f t = Vector2f())
        : center{c}
        , up{u}
        , radius{r}
        , timestamp{ts}
        , texture_coords{t}
        , materialID(-1)

    {
    }

    Cylinder() = default;

    Vector3f center;
    Vector3f up;
    float radius{0};
    float timestamp{0};
    Vector2f texture_coords{0.f, 0.f};
    int32 materialID = -1;
};
}

#endif // CYLINDER_H
