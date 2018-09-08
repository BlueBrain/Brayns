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
rtDeclareVariable(unsigned int, frame_number, , );
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
rtDeclareVariable(float, shadows, , );
rtDeclareVariable(float, soft_shadows, , );
rtDeclareVariable(float, ambient_occlusion_strength, , );
rtDeclareVariable(float4, jitter4, , );
rtDeclareVariable(float3, bg_color, , );

// Volume
rtBuffer<unsigned char> volumeData;
rtDeclareVariable(uint3, volumeDimensions, , );
rtDeclareVariable(float3, volumeOffset, , );
rtDeclareVariable(float3, volumeElementSpacing, , );
rtDeclareVariable(float, volumeEpsilon, , );
rtDeclareVariable(float, volumeDiag, , );
rtDeclareVariable(uint, volumeSamplesPerRay, , );

// Color map
rtBuffer<float4> colorMap;
rtBuffer<float3> emissionIntensityMap;
rtDeclareVariable(float, colorMapMinValue, , );
rtDeclareVariable(float, colorMapRange, , );
rtDeclareVariable(uint, colorMapSize, , );

rtBuffer<uchar4, 2> output_buffer;

const float epsilon = 0.01f;
const uint NB_MAX_SAMPLES_PER_RAY = 32;
const float DEFAULT_VOLUME_SHADOW_THRESHOLD = 0.5f;

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

static __device__ void composite(const float4& src, float4& dst)
{
    const float alphaCorrection =
        (float)NB_MAX_SAMPLES_PER_RAY / (float)volumeSamplesPerRay;
    const float alpha =
        1.f -
        pow(1.f - min(src.w, 1.f - 1.f / (float)colorMapSize), alphaCorrection);
    const float a = alpha * (1.f - dst.w);
    dst.x = dst.x + src.x * a;
    dst.y = dst.y + src.y * a;
    dst.z = dst.z + src.z * a;
    dst.w += (alpha * (1.f - dst.w));
}

static __device__ float getVolumeShadowContribution(const optix::Ray& volumeRay)
{
    float t = volumeDiag;
    float shadowIntensity = 0.f;
    while (t > volumeEpsilon && shadowIntensity < 1.f)
    {
        const float3 point = volumeRay.origin + volumeRay.direction * t;

        if (point.x > 0.f && point.x < volumeDimensions.x && point.y > 0.f &&
            point.y < volumeDimensions.y && point.z > 0.f &&
            point.z < volumeDimensions.z)
        {
            const ulong index =
                (ulong)((ulong)floor(point.x) +
                        (ulong)floor(point.y) * volumeDimensions.x +
                        (ulong)floor(point.z) * volumeDimensions.x *
                            volumeDimensions.y);
            const unsigned char voxelValue = volumeData[index];

            const float normalizedValue =
                (float)colorMapSize * ((float)voxelValue - colorMapMinValue) /
                colorMapRange;
            const float4 colorMapColor = colorMap[normalizedValue];
            if (colorMapColor.w > DEFAULT_VOLUME_SHADOW_THRESHOLD)
                shadowIntensity = 1.f;
        }
        t -= volumeEpsilon;
    }
    return shadowIntensity;
}

static __device__ float4 getVolumeContribution(const optix::Ray& volumeRay,
                                               const float hit)
{
    if (colorMapSize == 0)
        return make_float4(0.f, 1.f, 0.f, 0.f);

    float4 pathColor = make_float4(0.f);

    optix::size_t2 screen = output_buffer.size();
    unsigned int seed =
        tea<16>(screen.x * launch_index.y + launch_index.x, frame_number);
    const float delta = (float)(seed % (int)(volumeEpsilon + 1));

    float t = hit + volumeEpsilon - delta;
    const float tMax = hit - volumeEpsilon + volumeDiag;

    while (t < tMax && pathColor.w < 1.f)
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

            const float normalizedValue =
                ((float)voxelValue - colorMapMinValue) / colorMapRange;

            float4 colorMapColor;
            if (normalizedValue < 0.f)
                colorMapColor = colorMap[0];
            else if (normalizedValue >= 1.f)
                colorMapColor = colorMap[colorMapSize - 1];
            else
                colorMapColor = colorMap[colorMapSize * normalizedValue];

            const float4 emissionIntensity =
                make_float4(emissionIntensityMap[normalizedValue], 0.f);
            float4 voxelColor =
                fmaxf(make_float4(0.f), emissionIntensity + colorMapColor);

            // Determine light contribution
            if (shadows > 0.f && voxelColor.w > 0.1f)
            {
                float shadowIntensity = 0.f;
                for (int i = 0; i < lights.size(); ++i)
                {
                    BasicLight light = lights[i];
                    optix::Ray lightRay = volumeRay;
                    if (light.type == 0)
                    {
                        // Point light
                        float3 pos = light.pos;
                        if (soft_shadows > 0.f)
                            // Soft shadows
                            pos += soft_shadows * make_float3(rnd(seed) - 0.5f,
                                                              rnd(seed) - 0.5f,
                                                              rnd(seed) - 0.5f);
                        lightRay.origin = pos;
                        lightRay.direction = optix::normalize(pos - point);
                    }
                    else
                    {
                        // Directional light
                        float3 L = light.dir;
                        if (soft_shadows > 0.f)
                            // Soft shadows
                            L += soft_shadows * make_float3(rnd(seed) - 0.5f,
                                                            rnd(seed) - 0.5f,
                                                            rnd(seed) - 0.5f);
                        lightRay.origin = point;
                        lightRay.direction = optix::normalize(-1.f * L);
                    }

                    shadowIntensity += getVolumeShadowContribution(lightRay);
                }
                shadowIntensity = 1.f - shadows * shadowIntensity;
                voxelColor.x = voxelColor.x * shadowIntensity;
                voxelColor.y = voxelColor.y * shadowIntensity;
                voxelColor.z = voxelColor.z * shadowIntensity;
            }
            composite(voxelColor, pathColor);
        }
        t += volumeEpsilon;
    }
    return make_float4(max(0.f, min(1.f, pathColor.z)),
                       max(0.f, min(1.f, pathColor.y)),
                       max(0.f, min(1.f, pathColor.x)),
                       max(0.f, min(1.f, pathColor.w)));
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
                                  float p_glossiness,
                                  unsigned int p_shadindMode, float3 p_normal,
                                  float p_ray_tmax)
{
    float3 result = make_float3(0.f);
    // Randomness
    optix::size_t2 screen = output_buffer.size();
    unsigned int seed =
        tea<16>(screen.x * launch_index.y + launch_index.x, frame_number);

    // Glossiness
    if (p_glossiness < 1.f)
        p_normal = optix::normalize(
            p_normal + (1.f - p_glossiness) * make_float3(rnd(seed) - 0.5f,
                                                          rnd(seed) - 0.5f,
                                                          rnd(seed) - 0.5f));
    // Volume
    float4 volumeColor = make_float4(0.f, 0.f, 0.f, 0.f);
    if (volumeDiag != 0.f)
        volumeColor = getVolumeContribution(ray, t_hit);

    // Surface
    float3 hit_point = ray.origin + t_hit * ray.direction;
    float3 light_attenuation = make_float3(1.f - volumeColor.w);

    if (fmaxf(p_Ko) < epsilon)
        result += p_Ko * p_Kd;
    else if (p_shadindMode == electron)
        result +=
            p_Kd * (1.f - abs(optix::dot(optix::normalize(hit_point - eye),
                                         p_normal)));
    else if (p_shadindMode == none)
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
                if (shadows > 0.f && soft_shadows > 0.f)
                    // Soft shadows
                    pos += soft_shadows * make_float3(rnd(seed) - 0.5f,
                                                      rnd(seed) - 0.5f,
                                                      rnd(seed) - 0.5f);
                L = optix::normalize(pos - hit_point);
                Ldist = optix::length(pos - hit_point);
            }
            else
            {
                // Directional light
                L = -light.dir;
                if (shadows > 0.f && soft_shadows > 0.f)
                    // Soft shadows
                    L += soft_shadows * make_float3(rnd(seed) - 0.5f,
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
                        ray.origin + t_hit * ray.direction + scene_epsilon * L;
                    optix::Ray shadow_ray =
                        optix::make_Ray(hit_point, L, shadow_ray_type,
                                        scene_epsilon, Ldist);
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

        if (new_prd.importance > epsilon && new_prd.depth <= max_depth)
        {
            float3 R = optix::reflect(ray.direction, p_normal);
            hit_point = ray.origin + t_hit * ray.direction + scene_epsilon * R;
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

        if (new_prd.importance > epsilon && new_prd.depth <= max_depth)
        {
            float3 R = refractedVector(ray.direction, p_normal,
                                       p_refractionIndex, 1.f);
            hit_point = ray.origin + t_hit * ray.direction + scene_epsilon * R;
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
            hit_point =
                ray.origin + t_hit * ray.direction + scene_epsilon * aa_normal;
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
            if (new_prd.importance > epsilon && new_prd.depth <= max_depth)
            {
                float3 ra_normal = optix::normalize(
                    make_float3(rnd(seed) - 0.5f, rnd(seed) - 0.5f,
                                rnd(seed) - 0.5f));
                if (optix::dot(ra_normal, p_normal) < 0.f)
                    ra_normal = -ra_normal;
                hit_point = ray.origin + t_hit * ray.direction +
                            scene_epsilon * ra_normal;
                optix::Ray ra_ray =
                    optix::make_Ray(hit_point, ra_normal, radiance_ray_type,
                                    scene_epsilon, p_ray_tmax);
                rtTrace(top_shadower, ra_ray, new_prd);
                result += light_attenuation * ambient_occlusion_strength *
                          new_prd.result;
            }
        }
    }

    const float4 surfaceColor = make_float4(result, 1.f);
    composite(surfaceColor, volumeColor);

    prd.result = make_float3(volumeColor);
}
