/* Copyright (c) 2019, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
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

#include <optix_world.h>

#include "Helpers.h"

struct PerRayData_radiance
{
    float3 result;
    float importance;
    int depth;
};

rtDeclareVariable(optix::Ray, ray, rtCurrentRay, );
rtDeclareVariable(float3, bgColor, , );
rtDeclareVariable(int, envmap, , );
rtDeclareVariable(PerRayData_radiance, prd_radiance, rtPayload, );
rtDeclareVariable(uint, use_envmap, , );

RT_PROGRAM void envmap_miss()
{
    if (use_envmap)
    {
        const float2 uv = getEquirectangularUV(ray.direction);
        prd_radiance.result = linearToSRGB(
            tonemap(make_float3(optix::rtTex2D<float4>(envmap, uv.x, uv.y))));
        return;
    }
    prd_radiance.result = bgColor;
}
