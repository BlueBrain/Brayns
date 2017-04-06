/*
 * Copyright (c) 2016, NVIDIA CORPORATION. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  * Neither the name of NVIDIA CORPORATION nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Cyrille Favreau <cyrille.favreau@epfl.ch>
 * Added support for electron shading, shadows, soft shadows and ambient
 * occlusion
 */

#include "../CommonStructs.h"
#include "../Helpers.h"
#include "../Random.h"
#include <optix_world.h>

struct PerRayData_radiance
{
    float3 result;
    float importance;
    int depth;
};

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
rtDeclareVariable(unsigned int, max_depth, , );
rtBuffer<BasicLight> lights;
rtDeclareVariable(float3, ambient_light_color, , );
rtDeclareVariable(unsigned int, radiance_ray_type, , );
rtDeclareVariable(unsigned int, shadow_ray_type, , );
rtDeclareVariable(float, scene_epsilon, , );
rtDeclareVariable(rtObject, top_object, , );
rtDeclareVariable(rtObject, top_shadower, , );
rtDeclareVariable(float3, eye, , );
rtDeclareVariable(uint, shading_enabled, , );
rtDeclareVariable(uint, shadows_enabled, , );
rtDeclareVariable(uint, soft_shadows_enabled, , );
rtDeclareVariable(float, ambient_occlusion_strength, , );
rtDeclareVariable(uint, electron_shading_enabled, , );
rtDeclareVariable(float4, jitter4, , );
rtDeclareVariable(float3, bg_color, , );

// Volume
rtBuffer<unsigned char> volumeData;
rtDeclareVariable(uint3, volumeDimensions, , );
rtDeclareVariable(float3, volumeOffset, , );
rtDeclareVariable(float3, volumeElementSpacing, , );
rtDeclareVariable(float, volumeEpsilon, , );
rtDeclareVariable(float, volumeDiag, , );

// Color map
rtBuffer<float4> colorMap;
rtBuffer<float3> emissionIntensityMap;
rtDeclareVariable(float, colorMapMinValue, , );
rtDeclareVariable(float, colorMapRange, , );
rtDeclareVariable(uint, colorMapSize, , );

rtBuffer<uchar4, 2> output_buffer;

const float defaultVolumeAlpha = 4.f;
const float epsilon = 0.01f;
const float magicExponent = 50.f;

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

static __device__ float4 getVolumeContribution(optix::Ray volumeRay,
                                               int iteration)
{
    float pathAlpha = 0.f;
    float3 pathColor = make_float3(0.f);
    float3 lightContribution = make_float3(1.f);

    optix::size_t2 screen = output_buffer.size();
    unsigned int seed =
        tea<16>(screen.x * launch_index.y + launch_index.x, frame);
    const float delta = (float)(seed % (int)(volumeEpsilon + 1));

    float t = volumeEpsilon - delta;
    const float tMax = t_hit - volumeEpsilon + volumeDiag;

    while (t < tMax && pathAlpha < 1.f)
    {
        const float x = rnd(seed) * volumeEpsilon;
        float3 point = ((volumeRay.origin + volumeRay.direction * (t + x)) -
                        volumeOffset) /
                       volumeElementSpacing;

        if (point.x > 0.f && point.x < volumeDimensions.x && point.y > 0.f &&
            point.y < volumeDimensions.y && point.z > 0.f &&
            point.z < volumeDimensions.z)
        {
            ulong index = (ulong)((ulong)floor(point.x) +
                                  (ulong)floor(point.y) * volumeDimensions.x +
                                  (ulong)floor(point.z) * volumeDimensions.x *
                                      volumeDimensions.y);

            const unsigned char voxelValue = volumeData[index];

            // Determine light contribution
            if (shadows_enabled == 1 && iteration > 0)
            {
                unsigned int num_lights = lights.size();
                for (int i = 0; i < num_lights; ++i)
                {
                    optix::Ray lightRay = volumeRay;
                    lightRay.origin = point;

                    BasicLight light = lights[i];
                    if (light.type == 0)
                    {
                        // Point light
                        float3 pos = light.pos;
                        if (soft_shadows_enabled == 1)
                            // Soft shadows
                            pos += 5.f * make_float3(rnd(seed) - 0.5f,
                                                     rnd(seed) - 0.5f,
                                                     rnd(seed) - 0.5f);
                        lightRay.direction = optix::normalize(pos - point);
                    }
                    else
                    {
                        // Directional light
                        float3 L = -light.dir;
                        if (soft_shadows_enabled == 1)
                            // Soft shadows
                            L += 0.05f * make_float3(rnd(seed) - 0.5f,
                                                     rnd(seed) - 0.5f,
                                                     rnd(seed) - 0.5f);
                        lightRay.direction = optix::normalize(L);
                    }
                    const float4 voxelColor =
                        getVolumeContribution(lightRay, iteration - 1);
                    lightContribution =
                        lightContribution - make_float3(voxelColor);
                    pathAlpha += voxelColor.w;
                }
            }
            lightContribution = make_float3(max(0.f, lightContribution.z),
                                            max(0.f, lightContribution.y),
                                            max(0.f, lightContribution.x));
            pathAlpha = min(1.f, pathAlpha);

            if (colorMapSize != 0)
            {
                const float normalizedValue = colorMapSize *
                                              (voxelValue - colorMapMinValue) /
                                              colorMapRange;
                const float4 colorMapColor = colorMap[normalizedValue];
                const float3 emissionIntensity =
                    emissionIntensityMap[normalizedValue];

                const float3 voxelColor =
                    (make_float3(emissionIntensity.z, emissionIntensity.y,
                                 emissionIntensity.x) +
                     lightContribution) *
                    make_float3(colorMapColor.z, colorMapColor.y,
                                colorMapColor.x);

                const float alphaMagic =
                    magicExponent * volumeEpsilon / volumeDiag;
                const float alpha =
                    1.f - pow(1.f - min(colorMapColor.w,
                                        1.f - 1.f / (float)colorMapSize),
                              alphaMagic);
                pathColor =
                    pathColor + (voxelColor * alpha * (1.f - pathAlpha));
                pathAlpha = pathAlpha + (alpha * (1.f - pathAlpha));
            }
            else
            {
                const float3 voxelColor = {point.x, point.y, point.z};
                const float alpha = 1.f;
                pathColor =
                    pathColor + (voxelColor * alpha * (1.f - pathAlpha));
                pathAlpha = pathAlpha + (alpha * (1.f - pathAlpha));
            }
        }
        t += volumeEpsilon;
    }
    if (shadows_enabled == 1 && iteration > 0)
        pathColor = pathColor * defaultVolumeAlpha;
    return make_float4(min(1.f, pathColor.x), min(1.f, pathColor.y),
                       min(1.f, pathColor.z), min(1.f, pathAlpha));
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
                                  float3 p_normal, float p_ray_tmax)
{
    // Ambient contribution
    float3 result = {0.f, 0.f, 0.f};

    // Volume
    float4 volumeValue = {0.f, 0.f, 0.f, 0.f};
    if (volumeDiag != 0.f)
    {
        optix::Ray volumeRay = ray;
        volumeValue = getVolumeContribution(volumeRay, 1);
        result.x += volumeValue.x;
        result.y += volumeValue.y;
        result.z += volumeValue.z;
        result += (1.f - volumeValue.w) * bg_color;
    }

    // Surface
    float3 hit_point = ray.origin + (t_hit + scene_epsilon) * ray.direction;
    optix::size_t2 screen = output_buffer.size();
    unsigned int seed =
        tea<16>(screen.x * launch_index.y + launch_index.x, frame);

    float3 light_attenuation = make_float3(1.f - volumeValue.w);

    if (fmaxf(p_Ko) < epsilon)
        result += p_Ko * p_Kd;
    else if (electron_shading_enabled == 1)
        result +=
            p_Kd * (1.f - abs(optix::dot(optix::normalize(hit_point - eye),
                                         p_normal)));
    else if (shading_enabled == 0)
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
            if (light.type == 0)
            {
                // Point light
                float3 pos = light.pos;
                if (soft_shadows_enabled == 1)
                    // Soft shadows
                    pos += 5.f * make_float3(rnd(seed) - 0.5f, rnd(seed) - 0.5f,
                                             rnd(seed) - 0.5f);
                L = optix::normalize(pos - hit_point);
                Ldist = optix::length(pos - hit_point);
            }
            else
            {
                // Directional light
                L = -light.dir;
                if (soft_shadows_enabled == 1)
                    // Soft shadows
                    L += 0.05f * make_float3(rnd(seed) - 0.5f, rnd(seed) - 0.5f,
                                             rnd(seed) - 0.5f);
                L = optix::normalize(L);
            }
            float nDl = optix::dot(p_normal, L);

            // Shadows
            if (shadows_enabled)
            {
                light_attenuation = make_float3(static_cast<float>(nDl > 0.0f));
                if (nDl > 0.f && light.casts_shadow)
                {
                    PerRayData_shadow shadow_prd;
                    shadow_prd.attenuation = make_float3(1.0f);
                    optix::Ray shadow_ray =
                        optix::make_Ray(hit_point, L, shadow_ray_type,
                                        scene_epsilon, Ldist);
                    rtTrace(top_shadower, shadow_ray, shadow_prd);
                    light_attenuation = shadow_prd.attenuation;
                }
            }

            // If not completely shadowed, light the hit point
            if (fmaxf(light_attenuation) > 0.f)
            {
                float3 Lc = light.color * light_attenuation;

                result += p_Kd * nDl * Lc;

                float3 H = optix::normalize(L - ray.direction);
                float nDh = optix::dot(p_normal, H);
                if (nDh > 0)
                {
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

        if (new_prd.importance >= 0.f && new_prd.depth <= max_depth)
        {
            float3 R = optix::reflect(ray.direction, p_normal);
            optix::Ray refl_ray =
                optix::make_Ray(hit_point, R, radiance_ray_type, scene_epsilon,
                                p_ray_tmax);
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

        if (new_prd.importance > 0.f && new_prd.depth <= max_depth)
        {
            float3 R = refractedVector(ray.direction, p_normal,
                                       p_refractionIndex, 1.f);
            optix::Ray refr_ray =
                optix::make_Ray(hit_point, R, radiance_ray_type, scene_epsilon,
                                p_ray_tmax);
            rtTrace(top_object, refr_ray, new_prd);
            result += light_attenuation * (1.f - p_Ko) * new_prd.result;
        }
    }

    // Only opaque surfaces are affected by Global Illumination
    if (fmaxf(p_Ko) == 1.f)
    {
        // Ambient occlusion
        if (ambient_occlusion_strength > 0.f)
        {
            PerRayData_shadow aa_prd;
            aa_prd.attenuation = make_float3(1.f);

            float3 aa_normal =
                optix::normalize(make_float3(rnd(seed) - 0.5f, rnd(seed) - 0.5f,
                                             rnd(seed) - 0.5f));
            if (optix::dot(aa_normal, p_normal) < 0.f)
                aa_normal = -aa_normal;
            optix::Ray aa_ray =
                optix::make_Ray(hit_point, aa_normal, shadow_ray_type,
                                scene_epsilon, p_ray_tmax);
            rtTrace(top_shadower, aa_ray, aa_prd);
            result *=
                1.f - ambient_occlusion_strength * (1.f - aa_prd.attenuation);
        }

        // Radiosity
        if (ambient_occlusion_strength > 0.f && prd.depth == 0)
        {
            PerRayData_radiance new_prd;
            new_prd.importance = prd.importance;
            new_prd.depth = prd.depth + 1;
            if (new_prd.importance >= epsilon && new_prd.depth <= max_depth)
            {
                float3 ra_normal = optix::normalize(
                    make_float3(rnd(seed) - 0.5f, rnd(seed) - 0.5f,
                                rnd(seed) - 0.5f));
                if (optix::dot(ra_normal, p_normal) < 0.f)
                    ra_normal = -ra_normal;
                optix::Ray ra_ray =
                    optix::make_Ray(hit_point, ra_normal, radiance_ray_type,
                                    scene_epsilon, p_ray_tmax);
                rtTrace(top_shadower, ra_ray, new_prd);
                result += light_attenuation * ambient_occlusion_strength *
                          new_prd.result;
            }
        }
    }
    prd.result = result;
}
