/* Copyright (c) 2019, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Sebastien Speierer <sebastien.speierer@epfl.ch>
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

#include <brayns/common/types.h>

namespace brayns
{
struct SDFBezier
{
    uint64_t userData;
    Vector3f p0; // starting point
    Vector3f c0; // first control point
    float r0;    // starting radius
    Vector3f p1; // ending point
    Vector3f c1; // second control point
    float r1;    // ending radius
};

//////////////////////////////////////////////////////////////////////

inline float bezier(float t, float p0, float c0, float p1, float c1)
{
    float s = 1 - t;
    float s2 = s * s;
    float t2 = t * t;
    return s * s2 * p0 + 3 * s2 * t * c0 + 3 * s * t2 * c1 + t * t2 * p1;
}

//////////////////////////////////////////////////////////////////////

inline Boxd bezierBounds(const SDFBezier& bc)
{
    Boxd bounds;

    // see https://www.iquilezles.org/www/articles/bezierbbox/bezierbbox.htm

    // be conservative, use max radii
    float r = glm::max(bc.r0, bc.r1);

    // tight bounding box
    Vector3f mi = glm::min(bc.p0, bc.p1);
    Vector3f ma = glm::max(bc.p0, bc.p1);

    const Vector3f c = -1.0f * bc.p0 + bc.c0;
    const Vector3f b = 1.0f * bc.p0 - 2 * bc.c0 + bc.c1;
    const Vector3f a = -1.0f * bc.p0 + 3 * bc.c0 - 3 * bc.c1 + bc.p1;

    Vector3f h = b * b - a * c;

    if (h.x > 0.0f)
    {
        h.x = glm::sqrt(h.x);
        float t = (-b.x - h.x) / a.x;
        if (!glm::isnan(t) && t > 0.0f && t < 1.0f)
        {
            float q = bezier(t, bc.p0.x, bc.c0.x, bc.p1.x, bc.c1.x);
            mi.x = glm::min(mi.x, q);
            ma.x = glm::max(ma.x, q);
        }

        t = (-b.x + h.x) / a.x;
        if (!glm::isnan(t) && t > 0.0f && t < 1.0f)
        {
            float q = bezier(t, bc.p0.x, bc.c0.x, bc.p1.x, bc.c1.x);
            mi.x = glm::min(mi.x, q);
            ma.x = glm::max(ma.x, q);
        }
    }

    if (h.y > 0.0f)
    {
        h.y = glm::sqrt(h.y);
        float t = (-b.y - h.y) / a.y;
        if (!glm::isnan(t) && t > 0.0f && t < 1.0f)
        {
            float q = bezier(t, bc.p0.y, bc.c0.y, bc.p1.y, bc.c1.y);
            mi.y = glm::min(mi.y, q);
            ma.y = glm::max(ma.y, q);
        }
        t = (-b.y + h.y) / a.y;
        if (!glm::isnan(t) && t > 0.0f && t < 1.0f)
        {
            float q = bezier(t, bc.p0.y, bc.c0.y, bc.p1.y, bc.c1.y);
            mi.y = glm::min(mi.y, q);
            ma.y = glm::max(ma.y, q);
        }
    }

    if (h.z > 0.0f)
    {
        h.z = glm::sqrt(h.z);
        float t = (-b.z - h.z) / a.z;
        if (!glm::isnan(t) && t > 0.0f && t < 1.0f)
        {
            float q = bezier(t, bc.p0.z, bc.c0.z, bc.p1.z, bc.c1.z);
            mi.z = glm::min(mi.z, q);
            ma.z = glm::max(ma.z, q);
        }
        t = (-b.z + h.z) / a.z;
        if (!glm::isnan(t) && t > 0.0f && t < 1.0f)
        {
            float q = bezier(t, bc.p0.z, bc.c0.z, bc.p1.z, bc.c1.z);
            mi.z = glm::min(mi.z, q);
            ma.z = glm::max(ma.z, q);
        }
    }
    bounds.merge(mi - r);
    bounds.merge(ma + r);

    return bounds;
}

} // namespace brayns