/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Juan Hernando <juan.hernando@epfl.ch>
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

#include "utils.ih"

void clipRay(const uniform vec4f* clipPlanes, const unsigned int numClipPlanes,
             const varying vec3f& position, const varying vec3f& direction,
             varying float& near, varying float& far)
{
    for (int i = 0; i < numClipPlanes; ++i)
    {
        const vec3f normal = {
            clipPlanes[i].x, clipPlanes[i].y, clipPlanes[i].z};
        const float d = clipPlanes[i].w;

        const float distance = dot(normal, position) + d;
        const float cosAngle = dot(direction, normal);

        if (cosAngle == 0.f)
        {
            // This ray is parallel to the plane, checking if position is
            // on the visible side
            if (distance < 0)
            {
                near = far = 0.0;
                return;
            }
        }
        else
        {
            const float t = -distance / cosAngle;
            if (cosAngle > 0.f) // same direction plane
                near = max(near, t);
            else
                far = min(far, t);
        }
    }
}
