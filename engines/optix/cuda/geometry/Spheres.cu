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

using namespace optix;

#define OFFSET_USER_DATA 0
#define OFFSET_CENTER (OFFSET_USER_DATA + 2)
#define OFFSET_RADIUS (OFFSET_CENTER + 3)
#define OFFSET_TIMESTAMP (OFFSET_RADIUS + 1)
#define OFFSET_TEX_COORDS (OFFSET_TIMESTAMP + 1)

rtBuffer<float> spheres;

rtDeclareVariable(float3, geometric_normal, attribute geometric_normal, );
rtDeclareVariable(float3, shading_normal, attribute shading_normal, );
rtDeclareVariable(optix::Ray, ray, rtCurrentRay, );
rtDeclareVariable(unsigned int, sphere_size, , );
rtDeclareVariable(unsigned long, simulation_idx, attribute simulation_idx, );

template <bool use_robust_method>
static __device__ void intersect_sphere(int primIdx)
{
    const int idx = primIdx * sphere_size;

    const unsigned long userData =
        *((unsigned long*)(&spheres[idx + OFFSET_USER_DATA]));
    const float3 center = {spheres[idx + OFFSET_CENTER],
                           spheres[idx + OFFSET_CENTER + 1],
                           spheres[idx + OFFSET_CENTER + 2]};
    const float3 O = ray.origin - center;
    const float3 D = ray.direction;
    const float radius = spheres[idx + OFFSET_RADIUS];

    float b = dot(O, D);
    float c = dot(O, O) - radius * radius;
    float disc = b * b - c;
    if (disc > 0.0f)
    {
        float sdisc = sqrtf(disc);
        float root1 = (-b - sdisc);

        bool do_refine = false;

        float root11 = 0.0f;

        if (use_robust_method && fabsf(root1) > 10.f * radius)
            do_refine = true;

        if (do_refine)
        {
            // refine root1
            float3 O1 = O + root1 * ray.direction;
            b = dot(O1, D);
            c = dot(O1, O1) - radius * radius;
            disc = b * b - c;

            if (disc > 0.0f)
            {
                sdisc = sqrtf(disc);
                root11 = (-b - sdisc);
            }
        }

        bool check_second = true;
        if (rtPotentialIntersection(root1 + root11))
        {
            shading_normal = geometric_normal =
                (O + (root1 + root11) * D) / radius;
            simulation_idx = userData;
            if (rtReportIntersection(0))
                check_second = false;
        }
        if (check_second)
        {
            float root2 = (-b + sdisc) + (do_refine ? root1 : 0);
            if (rtPotentialIntersection(root2))
            {
                shading_normal = geometric_normal = (O + root2 * D) / radius;
                simulation_idx = userData;
                rtReportIntersection(0);
            }
        }
    }
}

RT_PROGRAM void intersect(int primIdx)
{
    intersect_sphere<false>(primIdx);
}

RT_PROGRAM void robust_intersect(int primIdx)
{
    intersect_sphere<true>(primIdx);
}

RT_PROGRAM void bounds(int primIdx, float result[6])
{
    const int idx = primIdx * sphere_size;
    const float3 cen = {spheres[idx + OFFSET_CENTER],
                        spheres[idx + OFFSET_CENTER + 1],
                        spheres[idx + OFFSET_CENTER + 2]};
    const float rad = spheres[idx + OFFSET_RADIUS];

    optix::Aabb* aabb = (optix::Aabb*)result;

    if (rad > 0.0f && !isinf(rad))
    {
        aabb->m_min = cen - rad;
        aabb->m_max = cen + rad;
    }
    else
    {
        aabb->invalidate();
    }
}
