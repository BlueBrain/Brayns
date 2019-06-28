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

#pragma once

#include "../../CommonStructs.h"
#include "../Helpers.h"
#include "../Random.h"
#include <optix_world.h>

struct PerRayData_shadow
{
    float3 attenuation;
};

// Scene
rtDeclareVariable(optix::Ray, ray, rtCurrentRay, );
rtDeclareVariable(float, t_hit, rtIntersectionDistance, );
rtDeclareVariable(PerRayData_radiance, prd, rtPayload, );
rtDeclareVariable(PerRayData_shadow, prd_shadow, rtPayload, );
rtDeclareVariable(unsigned int, frame, , );
rtDeclareVariable(uint2, launch_index, rtLaunchIndex, );
rtDeclareVariable(int, maxDepth, , );
rtDeclareVariable(unsigned int, radianceRayType, , );
rtDeclareVariable(unsigned int, shadowRayType, , );
rtDeclareVariable(float, sceneEpsilon, , );
rtDeclareVariable(rtObject, top_object, , );
rtDeclareVariable(rtObject, top_shadower, , );
rtDeclareVariable(float3, eye, , );
rtDeclareVariable(uint, shadingEnabled, , );
rtDeclareVariable(uint, electronShadingEnabled, , );
rtDeclareVariable(float4, jitter4, , );

// Lights
rtBuffer<BasicLight> lights;
rtDeclareVariable(float3, ambientLightColor, , );

// Rendering
rtDeclareVariable(float, shadows, , );
rtDeclareVariable(float, softShadows, , );
rtDeclareVariable(float, ambientOcclusionStrength, , );

rtBuffer<uchar4, 2> output_buffer;

const float epsilon = 0.01f;
const uint NB_MAX_SAMPLES_PER_RAY = 32;

static __device__ float3 refractedVector(const float3 direction,
                                         const float3 normal, const float n1,
                                         const float n2)
{
    if (n2 < epsilon || n1 == n2)
        return direction;
    const float eta = n1 / n2;
    const float cos1 = -optix::dot(direction, normal);
    const float cos2 = 1.f - eta * eta * (1.f - cos1 * cos1);
    if (cos2 > 0.f)
        return eta * direction + (eta * cos1 - sqrt(cos2)) * normal;
    return direction;
}

static __device__ void phongShadowed(float3 p_Ko)
{
    // this material is opaque, so it fully attenuates all shadow rays
    prd_shadow.attenuation = 1.f - p_Ko;
    rtTerminateRay();
}

static __device__ void phongShade(float3 p_Kd, float3 p_Ka, float3 p_Ks,
                                  float3 p_Kr, float3 p_Ko,
                                  float p_refractionIndex, float p_phong_exp,
                                  float p_glossiness, float3 p_normal,
                                  float p_ray_tmax)
{
    float3 result = make_float3(0.f);

    // Randomness
    optix::size_t2 screen = output_buffer.size();
    unsigned int seed =
        tea<16>(screen.x * launch_index.y + launch_index.x, frame);

    // Glossiness
    if (p_glossiness < 1.f)
        p_normal =
            optix::normalize(p_normal +
                             (1.f - p_glossiness) *
                                 make_float3(rnd(seed) - 0.5f, rnd(seed) - 0.5f,
                                             rnd(seed) - 0.5f));

    // Surface
    float3 hit_point = ray.origin + t_hit * ray.direction;
    float3 light_attenuation = make_float3(1.f);

    if (fmaxf(p_Ko) < epsilon)
        result += p_Ko * p_Kd;
    else if (electronShadingEnabled == 1)
        result +=
            p_Kd * (1.f - abs(optix::dot(optix::normalize(hit_point - eye),
                                         p_normal)));
    else if (shadingEnabled == 0)
        result += p_Kd;
    else
    {
        // compute direct lighting
        unsigned int num_lights = lights.size();
        for (int i = 0; i < num_lights; ++i)
        {
            BasicLight light = lights[i];

            float3 L;
            float Ldist = p_ray_tmax;
            if (light.type == BASIC_LIGHT_TYPE_POINT)
            {
                // Point light
                float3 pos = light.pos;
                if (shadows > 0.f && softShadows > 0.f)
                    // Soft shadows
                    pos += softShadows * make_float3(rnd(seed) - 0.5f,
                                                     rnd(seed) - 0.5f,
                                                     rnd(seed) - 0.5f);
                L = optix::normalize(pos - hit_point);
                Ldist = optix::length(pos - hit_point);
            }
            else
            {
                // Directional light
                L = -light.pos;
                if (shadows > 0.f && softShadows > 0.f)
                    // Soft shadows
                    L += softShadows * make_float3(rnd(seed) - 0.5f,
                                                   rnd(seed) - 0.5f,
                                                   rnd(seed) - 0.5f);
                L = optix::normalize(L);
            }
            float nDl = optix::dot(p_normal, L);

            // Shadows
            if (shadows > 0.f)
            {
                light_attenuation = make_float3(static_cast<float>(nDl > 0.0f));
                if (nDl > 0.f && light.casts_shadow)
                {
                    PerRayData_shadow shadow_prd;
                    shadow_prd.attenuation = make_float3(1.f);
                    hit_point =
                        ray.origin + t_hit * ray.direction + sceneEpsilon * L;
                    optix::Ray shadow_ray =
                        optix::make_Ray(hit_point, L, shadowRayType,
                                        sceneEpsilon, Ldist);
                    rtTrace(top_shadower, shadow_ray, shadow_prd);
                    // light_attenuation is zero if completely shadowed
                    light_attenuation = shadow_prd.attenuation;
                }
            }

            if (fmaxf(light_attenuation) < epsilon)
                light_attenuation = make_float3(1.f - shadows);

            // If not completely shadowed, light the hit point
            if (fmaxf(light_attenuation) > 0.f)
            {
                float3 Lc = light.color * light_attenuation;

                result += p_Kd * nDl * Lc;

                float3 H = optix::normalize(L - ray.direction);
                float nDh = optix::dot(p_normal, H);
                if (nDh > 0.f)
                {
                    // Specular
                    float power = pow(nDh, p_phong_exp);
                    result += p_Ks * power * Lc;
                }
            }
        }
    }

    // Reflection
    if (fmaxf(p_Kr) > epsilon)
    {
        PerRayData_radiance new_prd;
        new_prd.importance = prd.importance * optix::luminance(p_Kr);
        new_prd.depth = prd.depth + 1;

        if (new_prd.importance > epsilon && new_prd.depth <= maxDepth)
        {
            float3 R = optix::reflect(ray.direction, p_normal);
            hit_point = ray.origin + t_hit * ray.direction + sceneEpsilon * R;
            optix::Ray refl_ray = optix::make_Ray(hit_point, R, radianceRayType,
                                                  sceneEpsilon, p_ray_tmax);
            rtTrace(top_object, refl_ray, new_prd);
            result += light_attenuation * p_Kr * new_prd.result;
        }
    }

    // Refraction
    if (fmaxf(p_Ko) < 1.f)
    {
        PerRayData_radiance new_prd;
        new_prd.importance =
            max(0.f, prd.importance - optix::luminance(1.f - p_Ko));
        new_prd.depth = prd.depth + 1;

        if (new_prd.importance > epsilon && new_prd.depth <= maxDepth)
        {
            float3 R = refractedVector(ray.direction, p_normal,
                                       p_refractionIndex, 1.f);
            hit_point = ray.origin + t_hit * ray.direction + sceneEpsilon * R;
            optix::Ray refr_ray = optix::make_Ray(hit_point, R, radianceRayType,
                                                  sceneEpsilon, p_ray_tmax);
            rtTrace(top_object, refr_ray, new_prd);
            result += light_attenuation * (1.f - p_Ko) * new_prd.result;
        }
    }

    // Only opaque surfaces are affected by Global Illumination
    if (fmaxf(p_Ko) == 1.f)
    {
        // Ambient occlusion
        if (ambientOcclusionStrength > 0.f)
        {
            PerRayData_shadow aa_prd;
            aa_prd.attenuation = make_float3(1.f);

            float3 aa_normal =
                optix::normalize(make_float3(rnd(seed) - 0.5f, rnd(seed) - 0.5f,
                                             rnd(seed) - 0.5f));
            if (optix::dot(aa_normal, p_normal) < 0.f)
                aa_normal = -aa_normal;
            hit_point =
                ray.origin + t_hit * ray.direction + sceneEpsilon * aa_normal;
            optix::Ray aa_ray =
                optix::make_Ray(hit_point, aa_normal, shadowRayType,
                                sceneEpsilon, p_ray_tmax);
            rtTrace(top_shadower, aa_ray, aa_prd);
            result *=
                1.f - ambientOcclusionStrength * (1.f - aa_prd.attenuation);
        }

        // Radiosity
        if (ambientOcclusionStrength > 0.f && prd.depth == 0)
        {
            PerRayData_radiance new_prd;
            new_prd.importance = prd.importance;
            new_prd.depth = prd.depth + 1;
            if (new_prd.importance > epsilon && new_prd.depth <= maxDepth)
            {
                float3 ra_normal = optix::normalize(
                    make_float3(rnd(seed) - 0.5f, rnd(seed) - 0.5f,
                                rnd(seed) - 0.5f));
                if (optix::dot(ra_normal, p_normal) < 0.f)
                    ra_normal = -ra_normal;
                hit_point = ray.origin + t_hit * ray.direction +
                            sceneEpsilon * ra_normal;
                optix::Ray ra_ray =
                    optix::make_Ray(hit_point, ra_normal, radianceRayType,
                                    sceneEpsilon, p_ray_tmax);
                rtTrace(top_shadower, ra_ray, new_prd);
                result += light_attenuation * ambientOcclusionStrength *
                          new_prd.result;
            }
        }
    }

    const float4 surfaceColor = make_float4(result, 1.f);
    prd.result = make_float3(surfaceColor);
}
