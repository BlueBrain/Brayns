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

#include "IntersectionRefinement.h"
#include <optix.h>
#include <optixu/optixu_aabb_namespace.h>
#include <optixu/optixu_math_namespace.h>
#include <optixu/optixu_matrix_namespace.h>

using namespace optix;

// This is to be plugged into an RTgeometry object to represent
// a triangle mesh with a vertex buffer of triangle soup (triangle list)
// with an interleaved position, normal, texturecoordinate layout.

rtBuffer<float3> vertices_buffer;
rtBuffer<float3> normal_buffer;
rtBuffer<float2> texcoord_buffer;
rtBuffer<int3> indices_buffer;

rtDeclareVariable(float3, texcoord, attribute texcoord, );
rtDeclareVariable(float3, geometric_normal, attribute geometric_normal, );
rtDeclareVariable(float3, shading_normal, attribute shading_normal, );

rtDeclareVariable(float3, back_hit_point, attribute back_hit_point, );
rtDeclareVariable(float3, front_hit_point, attribute front_hit_point, );

rtDeclareVariable(optix::Ray, ray, rtCurrentRay, );
rtDeclareVariable(unsigned long, simulation_idx, attribute simulation_idx, );

template <bool DO_REFINE>
static __device__ void meshIntersect(int primIdx)
{
    const int3 v_idx = indices_buffer[primIdx];

    const float3 p0 = vertices_buffer[v_idx.x];
    const float3 p1 = vertices_buffer[v_idx.y];
    const float3 p2 = vertices_buffer[v_idx.z];

    // Intersect ray with triangle
    float3 n;
    float t, beta, gamma;
    if (intersect_triangle(ray, p0, p1, p2, n, t, beta, gamma))
    {
        if (rtPotentialIntersection(t))
        {
            geometric_normal = normalize(n);
            if (normal_buffer.size() == 0)
                shading_normal = geometric_normal;
            else
            {
                float3 n0 = normal_buffer[v_idx.x];
                float3 n1 = normal_buffer[v_idx.y];
                float3 n2 = normal_buffer[v_idx.z];
                shading_normal = normalize(n1 * beta + n2 * gamma +
                                           n0 * (1.f - beta - gamma));
            }

            if (texcoord_buffer.size() == 0)
                texcoord = make_float3(0.f, 0.f, 0.f);
            else
            {
                float2 t0 = texcoord_buffer[v_idx.x];
                float2 t1 = texcoord_buffer[v_idx.y];
                float2 t2 = texcoord_buffer[v_idx.z];
                texcoord = make_float3(t1 * beta + t2 * gamma +
                                       t0 * (1.f - beta - gamma));
            }

            if (DO_REFINE)
                refine_and_offset_hitpoint(ray.origin + t * ray.direction,
                                           ray.direction, geometric_normal, p0,
                                           back_hit_point, front_hit_point);
            simulation_idx = 0;
            rtReportIntersection(0);
        }
    }
}

RT_PROGRAM void intersect(int primIdx)
{
    meshIntersect<false>(primIdx);
}

RT_PROGRAM void intersect_refine(int primIdx)
{
    meshIntersect<true>(primIdx);
}

RT_PROGRAM void bounds(int primIdx, float result[6])
{
    const int3 v_idx = indices_buffer[primIdx];

    const float3 v0 = vertices_buffer[v_idx.x];
    const float3 v1 = vertices_buffer[v_idx.y];
    const float3 v2 = vertices_buffer[v_idx.z];
    const float area = length(cross(v1 - v0, v2 - v0));

    optix::Aabb* aabb = (optix::Aabb*)result;

    if (area > 0.0f && !isinf(area))
    {
        aabb->m_min = fminf(fminf(v0, v1), v2);
        aabb->m_max = fmaxf(fmaxf(v0, v1), v2);
    }
    else
    {
        aabb->invalidate();
    }
}
