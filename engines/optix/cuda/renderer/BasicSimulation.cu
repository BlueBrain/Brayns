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
rtTextureSampler<float4, 2> diffuse_map;
rtDeclareVariable(float3, texcoord, attribute texcoord, );

// Simulation data
rtDeclareVariable(unsigned int, use_simulation_data, , );
rtBuffer<float3> colors;
rtBuffer<float> opacities;
rtDeclareVariable(float2, value_range, , );
rtBuffer<float> simulation_data;
rtDeclareVariable(unsigned long, user_data, attribute user_data, );

struct InterpolatedValue
{
    float influence_low;
    float influence_high;
    int idx_low;
    int idx_high;
};

// Finds between what pair of values a given value is and how much influence
// each of these values have
static __device__ inline InterpolatedValue interpolateValues(
    const float v_min, const float v_max, const float value,
    const int num_values)
{
    InterpolatedValue result;

    const float v_clamped = min(v_max, max(v_min, value));
    const float range_per_value = (v_max - v_min) / (num_values - 1);
    const float idx_value = (v_clamped - v_min) / range_per_value;

    // Indices of the two values to interpolate between
    const int idx_value_low = max(0, int(floor(idx_value)));
    const int idx_value_high = min(num_values - 1, int(ceil(idx_value)));

    result.idx_low = idx_value_low;
    result.idx_high = idx_value_high;

    if (idx_value_low == idx_value_high)
    {
        // Since both values are the same we split the influence
        result.influence_low = 0.5f;
        result.influence_high = 0.5f;
    }
    else
    {
        const float v_low = v_min + float(idx_value_low) * range_per_value;
        const float v_high = v_min + float(idx_value_high) * range_per_value;

        result.influence_low = 1.0f - abs(v_clamped - v_low) / range_per_value;
        result.influence_high =
            1.0f - abs(v_clamped - v_high) / range_per_value;
    }

    return result;
}

static __device__ inline float3 calcTransferFunctionColor()
{
    const float3 WHITE = make_float3(1.f, 1.f, 1.f);
    float3 color_opaque;
    float opacity;

    const float x_min = value_range.x;
    const float x_max = value_range.y;
    const float x_value = simulation_data[user_data];

    {
        const int num_colors = colors.size() / (sizeof(float3));
        const InterpolatedValue iv =
            interpolateValues(x_min, x_max, x_value, num_colors);
        color_opaque = colors[iv.idx_low] * iv.influence_low +
                       colors[iv.idx_high] * iv.influence_high;
    }

    {
        const int num_opacities = opacities.size() / (sizeof(float));
        const InterpolatedValue iv =
            interpolateValues(x_min, x_max, x_value, num_opacities);
        opacity = opacities[iv.idx_low] * iv.influence_low +
                  opacities[iv.idx_high] * iv.influence_high;
    }

    return opacity * color_opaque + (1.0f - opacity) * WHITE;
}

static __device__ inline void shade(bool textured)
{
    float3 world_shading_normal =
        optix::normalize(rtTransformNormal(RT_OBJECT_TO_WORLD, shading_normal));
    float3 world_geometric_normal = optix::normalize(
        rtTransformNormal(RT_OBJECT_TO_WORLD, geometric_normal));

    float3 p_normal = optix::faceforward(world_shading_normal, -ray.direction,
                                         world_geometric_normal);

    const float3 p_Kd =
        textured ? make_float3(tex2D(diffuse_map, texcoord.x, texcoord.y)) : Kd;

    prd.result = p_Kd * max(0.f, optix::dot(-ray.direction, p_normal));
    const float3 col = use_simulation_data ? calcTransferFunctionColor() : Kd;
    prd.result = col * max(0.f, optix::dot(-ray.direction, p_normal));
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
