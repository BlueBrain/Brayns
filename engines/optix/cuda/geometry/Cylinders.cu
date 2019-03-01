/* Copyright (c) 2015-2016, EPFL/Blue Brain Project
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

#include <optix_world.h>

using namespace optix;

#define OFFSET_USER_DATA 0
#define OFFSET_CENTER (OFFSET_USER_DATA + 2)
#define OFFSET_UP (OFFSET_CENTER + 3)
#define OFFSET_RADIUS (OFFSET_UP + 3)
#define OFFSET_TIMESTAMP (OFFSET_RADIUS + 1)
#define OFFSET_TEX_COORDS (OFFSET_TIMESTAMP + 1)

// Global variables
rtBuffer<float> cylinders;

// Geometry specific variables
rtDeclareVariable(float3, geometric_normal, attribute geometric_normal, );
rtDeclareVariable(float3, shading_normal, attribute shading_normal, );
rtDeclareVariable(optix::Ray, ray, rtCurrentRay, );
rtDeclareVariable(unsigned int, cylinder_size, , );
rtDeclareVariable(unsigned long, simulation_idx, attribute simulation_idx, );

template <bool use_robust_method>
static __device__ void intersect_cylinder(int primIdx)
{
    const int idx = primIdx * cylinder_size;
    const unsigned long userData =
        *((unsigned long*)(&cylinders[idx + OFFSET_USER_DATA]));

    const float3 v0 = {cylinders[idx + OFFSET_CENTER],
                       cylinders[idx + OFFSET_CENTER + 1],
                       cylinders[idx + OFFSET_CENTER + 2]};
    const float3 v1 = {cylinders[idx + OFFSET_UP],
                       cylinders[idx + OFFSET_UP + 1],
                       cylinders[idx + OFFSET_UP + 2]};
    const float radius = cylinders[idx + OFFSET_RADIUS];

    const float3 A = v0 - ray.origin;
    const float3 B = v1 - ray.origin;

    const float3 O = make_float3(0.f);
    const float3 V = ray.direction;

    const float3 AB = B - A;
    const float3 AO = O - A;

    const float3 AOxAB = cross(AO, AB);
    const float3 VxAB = cross(V, AB);
    const float ab2 = dot(AB, AB);
    const float a = dot(VxAB, VxAB);
    const float b = 2.f * dot(VxAB, AOxAB);
    const float c = dot(AOxAB, AOxAB) - (radius * radius * ab2);

    const float radical = b * b - 4.f * a * c;
    if (radical >= 0.f)
    {
        // clip to near and far cap of cylinder
        const float tA = dot(AB, A) / dot(V, AB);
        const float tB = dot(AB, B) / dot(V, AB);
        // const float tAB0 = max( 0.f, min( tA, tB ));
        // const float tAB1 = min( RT_DEFAULT_MAX, max( tA, tB ));
        const float tAB0 = min(tA, tB);
        const float tAB1 = max(tA, tB);

        const float srad = sqrt(radical);

        const float t_in = (-b - srad) / (2.f * a);

        bool check_second = true;
        if (t_in >= tAB0 && t_in <= tAB1)
        {
            if (rtPotentialIntersection(t_in))
            {
                const float3 P = ray.origin + t_in * ray.direction - v0;
                const float3 V = cross(P, AB);
                geometric_normal = shading_normal = cross(AB, V);
                simulation_idx = userData;
                if (rtReportIntersection(0))
                    check_second = false;
            }
        }

        if (check_second)
        {
            const float t_out = (-b + srad) / (2.f * a);
            if (t_out >= tAB0 && t_out <= tAB1)
            {
                if (rtPotentialIntersection(t_out))
                {
                    const float3 P = t_out * ray.direction - A;
                    const float3 V = cross(P, AB);
                    geometric_normal = shading_normal = cross(AB, V);
                    simulation_idx = userData;
                    rtReportIntersection(0);
                }
            }
        }
    }
}

RT_PROGRAM void intersect(int primIdx)
{
    intersect_cylinder<false>(primIdx);
}

RT_PROGRAM void robust_intersect(int primIdx)
{
    intersect_cylinder<true>(primIdx);
}

RT_PROGRAM void bounds(int primIdx, float result[6])
{
    const int idx = primIdx * cylinder_size;
    const float3 v0 = {cylinders[idx + OFFSET_CENTER],
                       cylinders[idx + OFFSET_CENTER + 1],
                       cylinders[idx + OFFSET_CENTER + 2]};
    const float3 v1 = {cylinders[idx + OFFSET_UP],
                       cylinders[idx + OFFSET_UP + 1],
                       cylinders[idx + OFFSET_UP + 2]};
    const float radius = cylinders[idx + OFFSET_RADIUS];

    optix::Aabb* aabb = (optix::Aabb*)result;

    if (radius > 0.f && !isinf(radius))
    {
        aabb->m_min = fminf(v0, v1) - radius;
        aabb->m_max = fmaxf(v0, v1) + radius;
    }
    else
        aabb->invalidate();
}
