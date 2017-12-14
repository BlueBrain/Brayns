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

#ifndef SPHERE_H
#define SPHERE_H

#include <brayns/common/types.h>

namespace brayns
{
struct Sphere
{
    Sphere(const Vector3f c, float r, float ts = 0.f,
           const Vector2f v = Vector2f(0.f, 0.f))
        : center{c}
        , radius{r}
        , timestamp{ts}
        , values{v}
    {
    }

    Vector3f center;
    float radius{0};
    float timestamp{0};
    Vector2f values;
};
}
#endif // SPHERE_H
