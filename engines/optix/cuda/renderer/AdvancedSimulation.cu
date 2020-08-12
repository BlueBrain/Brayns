/* Copyright (c) 2020, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
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

#include "AdvancedSimulation.h"
#include "TransferFunction.h"

#include <optix.h>
#include <optixu/optixu_math_namespace.h>

using namespace optix;

// System
rtDeclareVariable(float3, bad_color, , );

// Material attributes
rtDeclareVariable(float3, Ka, , );
rtDeclareVariable(float3, Kd, , );
rtDeclareVariable(float3, Ks, , );
rtDeclareVariable(float3, Kr, , );
rtDeclareVariable(float3, Ko, , );
rtDeclareVariable(float, glossiness, , );
rtDeclareVariable(float, refraction_index, , );
rtDeclareVariable(float, phong_exp, , );

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

RT_PROGRAM void any_hit_shadow()
{
    phongShadowed(Ko);
}

static __device__ inline void shade(bool textured)
{
    float3 world_shading_normal =
        normalize(rtTransformNormal(RT_OBJECT_TO_WORLD, shading_normal));
    float3 world_geometric_normal =
        normalize(rtTransformNormal(RT_OBJECT_TO_WORLD, geometric_normal));

    float3 ffnormal = faceforward(world_shading_normal, -ray.direction,
                                  world_geometric_normal);

    float3 p_Kd = Kd;
    if (simulation_data.size() > 0)
        p_Kd = calcTransferFunctionColor(value_range.x, value_range.y,
                                         simulation_data[simulation_idx],
                                         colors, opacities);
    else if (textured)
        p_Kd = make_float3(
            optix::rtTex2D<float4>(albedoMetallic_map, texcoord.x, texcoord.y));

    phongShade(p_Kd, Ka, Ks, Kr, Ko, refraction_index, phong_exp, glossiness,
               ffnormal);
}

RT_PROGRAM void closest_hit_radiance()
{
    shade(false);
}

RT_PROGRAM void closest_hit_radiance_textured()
{
    shade(true);
}

RT_PROGRAM void exception()
{
    output_buffer[launch_index] = make_color( bad_color );
}
