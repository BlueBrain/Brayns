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

#include "../../CommonStructs.h"
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

rtDeclareVariable(float2, texcoord, attribute texcoord, );
rtDeclareVariable(float3, v0, attribute v0, );
rtDeclareVariable(float3, v1, attribute v1, );
rtDeclareVariable(float3, v2, attribute v2, );
rtDeclareVariable(float2, t0, attribute t0, );
rtDeclareVariable(float2, t1, attribute t1, );
rtDeclareVariable(float2, t2, attribute t2, );
rtDeclareVariable(float2, ddx, attribute ddx, );
rtDeclareVariable(float2, ddy, attribute ddy, );
rtDeclareVariable(float3, ddxWPos, attribute ddxWPos, );
rtDeclareVariable(float3, ddyWPos, attribute ddyWPos, );
rtDeclareVariable(float3, geometric_normal, attribute geometric_normal, );
rtDeclareVariable(float3, shading_normal, attribute shading_normal, );

rtDeclareVariable(float3, back_hit_point, attribute back_hit_point, );
rtDeclareVariable(float3, front_hit_point, attribute front_hit_point, );

rtDeclareVariable(optix::Ray, ray, rtCurrentRay, );
rtDeclareVariable(PerRayData_radiance, prd, rtPayload, );
rtDeclareVariable(unsigned long, simulation_idx, attribute simulation_idx, );

static __device__ void computeWPosDerivatives(float3& ddxwpos, float3& ddywpos,
                                              float3 p0, float3 p1, float3 p2,
                                              float2 betaDerivative,
                                              float2 gammaDerivative)
{
    ddxwpos = p1 * betaDerivative.x + p2 * gammaDerivative.x +
              p0 * (-betaDerivative.x - gammaDerivative.x);
    ddywpos = p1 * betaDerivative.y + p2 * gammaDerivative.y +
              p0 * (-betaDerivative.y - gammaDerivative.y);
}

static __device__ bool intersect_triangle_filtered(
    const Ray& ray, const float3& p0, const float3& p1, const float3& p2,
    const float3& rayDdx, const float3& rayDdy, float3& n, float& t,
    float& beta, float& gamma, float2& betaDerivative, float2& gammaDerivative)
{
    const float3 e0 = p1 - p0;
    const float3 e1 = p0 - p2;
    n = cross(e1, e0);

    const float3 g0 = p0 - ray.origin;

    const float NdotRay = dot(n, ray.direction);

    float3 e2 = g0 / NdotRay;
    float3 i = cross(ray.direction, e2);

    beta = dot(i, e1);
    gamma = dot(i, e0);
    t = dot(n, e2);

    if (!((t < ray.tmax) & (t > ray.tmin) & (beta >= 0.0f) & (gamma >= 0.0f) &
          (beta + gamma <= 1)))
        return false;

    const float3 differentialX =
        cross(rayDdx, e2) +
        cross(ray.direction, -e2 * dot(rayDdx, n) / NdotRay);
    const float3 differentialY =
        cross(rayDdy, e2) +
        cross(ray.direction, -e2 * dot(rayDdy, n) / NdotRay);
    betaDerivative.x = dot(differentialX, e1);
    betaDerivative.y = dot(differentialY, e1);
    gammaDerivative.x = dot(differentialX, e0);
    gammaDerivative.y = dot(differentialY, e0);

    return true;
}

template <bool DO_REFINE>
static __device__ void meshIntersect(int primIdx)
{
    const int3 v_idx = indices_buffer[primIdx];

    const float3 p0 = vertices_buffer[v_idx.x];
    const float3 p1 = vertices_buffer[v_idx.y];
    const float3 p2 = vertices_buffer[v_idx.z];

    // Intersect ray with triangle
    float3 n;
    float t;
    float beta, gamma;
    float2 betaDerivative, gammaDerivative;
    if (intersect_triangle_filtered(ray, p0, p1, p2, prd.rayDdx, prd.rayDdy, n,
                                    t, beta, gamma, betaDerivative,
                                    gammaDerivative))
    {
        if (rtPotentialIntersection(t))
        {
            v0 = p0;
            v1 = p1;
            v2 = p2;
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
            {
                texcoord = make_float2(0.f, 0.f);
                computeWPosDerivatives(ddxWPos, ddyWPos, p0, p1, p2,
                                       betaDerivative, gammaDerivative);
            }
            else
            {
                t0 = texcoord_buffer[v_idx.x];
                t1 = texcoord_buffer[v_idx.y];
                t2 = texcoord_buffer[v_idx.z];

                texcoord = t1 * beta + t2 * gamma + t0 * (1.f - beta - gamma);

                ddx = t1 * betaDerivative.x + t2 * gammaDerivative.x +
                      t0 * (-betaDerivative.x - gammaDerivative.x);
                ddy = t1 * betaDerivative.y + t2 * gammaDerivative.y +
                      t0 * (-betaDerivative.y - gammaDerivative.y);

                computeWPosDerivatives(ddxWPos, ddyWPos, p0, p1, p2,
                                       betaDerivative, gammaDerivative);
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
