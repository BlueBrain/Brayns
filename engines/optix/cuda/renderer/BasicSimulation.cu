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

#include "TransferFunction.h"

#include <optix_world.h>

struct PerRayData_radiance
{
    float3 result;
    float importance;
    int depth;
};

// Scene
rtDeclareVariable(optix::Ray, ray, rtCurrentRay, );
rtDeclareVariable(PerRayData_radiance, prd, rtPayload, );

// Material attributes
rtDeclareVariable(float3, Kd, , );

rtDeclareVariable(float3, geometric_normal, attribute geometric_normal, );
rtDeclareVariable(float3, shading_normal, attribute shading_normal, );

// Textures
rtDeclareVariable(int, albedoMetallic_map, , );
rtDeclareVariable(float2, texcoord, attribute texcoord, );

// Simulation data
rtBuffer<float3> colors;
rtBuffer<float> opacities;
rtDeclareVariable(float2, value_range, , );
rtBuffer<float> simulation_data;
rtDeclareVariable(unsigned long, simulation_idx, attribute simulation_idx, );

static __device__ inline void shade(bool textured)
{
    float3 world_shading_normal =
        optix::normalize(rtTransformNormal(RT_OBJECT_TO_WORLD, shading_normal));
    float3 world_geometric_normal = optix::normalize(
        rtTransformNormal(RT_OBJECT_TO_WORLD, geometric_normal));

    float3 p_normal = optix::faceforward(world_shading_normal, -ray.direction,
                                         world_geometric_normal);

    float3 p_Kd;
    if (simulation_data.size() > 0)
        p_Kd = calcTransferFunctionColor(value_range.x, value_range.y,
                                         simulation_data[simulation_idx],
                                         colors, opacities);
    else if (textured)
        p_Kd = make_float3(
            optix::rtTex2D<float4>(albedoMetallic_map, texcoord.x, texcoord.y));
    else
        p_Kd = Kd;

    prd.result = p_Kd * max(0.f, optix::dot(-ray.direction, p_normal));
}

RT_PROGRAM void any_hit_shadow()
{
    rtTerminateRay();
}

RT_PROGRAM void closest_hit_radiance()
{
    shade(false);
}

RT_PROGRAM void closest_hit_radiance_textured()
{
    shade(true);
}
