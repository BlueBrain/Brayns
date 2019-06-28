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
#include "../Helpers.h"
#include <optix_world.h>

using namespace optix;

// Scene
rtDeclareVariable(optix::Ray, ray, rtCurrentRay, );
rtDeclareVariable(PerRayData_radiance, prd, rtPayload, );
rtDeclareVariable(float, t_hit, rtIntersectionDistance, );

// Material attributes

rtDeclareVariable(float3, geometric_normal, attribute geometric_normal, );
rtDeclareVariable(float3, shading_normal, attribute shading_normal, );

// Textures
rtDeclareVariable(int, albedoMetallic_map, , );
rtDeclareVariable(int, normalRoughness_map, , );
rtDeclareVariable(float2, texcoord, attribute texcoord, );

// Lights
rtBuffer<BasicLight> lights;

// Vertices
rtDeclareVariable(float3, v0, attribute v0, );
rtDeclareVariable(float3, v1, attribute v1, );
rtDeclareVariable(float3, v2, attribute v2, );
rtDeclareVariable(float2, t0, attribute t0, );
rtDeclareVariable(float2, t1, attribute t1, );
rtDeclareVariable(float2, t2, attribute t2, );

rtDeclareVariable(float2, ddx, attribute ddx, );
rtDeclareVariable(float2, ddy, attribute ddy, );

rtDeclareVariable(uint, use_envmap, , );
rtDeclareVariable(int, envmap_radiance, , );
rtDeclareVariable(int, envmap_irradiance, , );
rtDeclareVariable(int, envmap_brdf_lut, , );
rtDeclareVariable(uint, radianceLODs, , );

static __device__ inline float calculateAttenuation(float3 WorldPos,
                                                    float3 lightPos)
{
    float distance = length(lightPos - WorldPos);
    return 1.0f / (distance * distance);
}

static __device__ inline float distributionGGX(float3 N, float3 H,
                                               float roughness)
{
    const float a = roughness * roughness;
    const float a2 = a * a;
    const float NdotH = max(dot(N, H), 0.0f);
    const float NdotH2 = NdotH * NdotH;

    float denom = (NdotH2 * (a2 - 1.0f) + 1.0f);
    denom = M_PIf * denom * denom;

    return a2 / denom;
}

static __device__ inline float GeometrySchlickGGX(float NdotV, float roughness)
{
    const float r = roughness + 1.0f;
    const float k = (r * r) / 8.0f;

    float denom = NdotV * (1.0 - k) + k;

    return NdotV / denom;
}

static __device__ inline float geometrySmith(float3 N, float3 V, float3 L,
                                             float roughness)
{
    const float NdotV = max(dot(N, V), 0.0f);
    const float NdotL = max(dot(N, L), 0.0f);
    const float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    const float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

static __device__ inline float3 fresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (make_float3(1.0f) - F0) * pow(1.0f - cosTheta, 5.0f);
}

static __device__ inline float3 fresnelSchlickRoughness(float cosTheta,
                                                        float3 F0,
                                                        float roughness)
{
    return F0 +
           (max(make_float3(1.0f - roughness), F0) - F0) *
               pow(1.0f - cosTheta, 5.0f);
}

static __device__ inline void shade()
{
    const float3 world_shading_normal =
        optix::normalize(rtTransformNormal(RT_OBJECT_TO_WORLD, shading_normal));

    float3 N = world_shading_normal;

    const float3 edge1 = v1 - v0;
    const float3 edge2 = v2 - v0;
    const float2 deltaUV1 = t1 - t0;
    const float2 deltaUV2 = t2 - t0;

    float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

    float3 tangent;
    tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
    tangent = normalize(rtTransformNormal(RT_OBJECT_TO_WORLD, tangent));

    tangent = normalize(tangent - dot(tangent, N) * N);

    float3 bitangent = cross(N, tangent);

    const float3 WorldPos = ray.origin + t_hit * ray.direction;
    const float3 V = -ray.direction;
    const float4 albedoMetallic =
        SRGBtoLinear(rtTex2DGrad<float4>(albedoMetallic_map, texcoord.x,
                                         texcoord.y, ddx, ddy));
    float3 albedo = make_float3(albedoMetallic);

    const float4 normalRoughness =
        rtTex2D<float4>(normalRoughness_map, texcoord.x, texcoord.y);
    const float roughness = max(normalRoughness.w, 0.05f);
    const float3 normal = make_float3(normalRoughness);
    optix::Matrix3x3 TBN;
    TBN.setCol(0, tangent);
    TBN.setCol(1, bitangent);
    TBN.setCol(2, N);

    N = normalize(TBN * (normal * 2.0f - 1.0f));

    const float3 F0 = lerp(make_float3(0.04f), albedo, albedoMetallic.w);

    float3 Lo = make_float3(0.0f);
    unsigned int num_lights = lights.size();
    for (int i = 0; i < num_lights; ++i)
    {
        // per-light radiance
        // const BasicLight& light = lights[i];
        BasicLight light = lights[i];
        light.pos = make_float3(0.5f, 1.0f, 1.5f);
        const float3 L = normalize(light.pos - WorldPos);
        const float3 H = normalize(V + L);
        const float attenuation = calculateAttenuation(WorldPos, light.pos);
        const float3 radiance =
            light.color * attenuation * 20.0f;

        // cook-torrance brdf
        const float NDF = distributionGGX(N, H, roughness);
        const float G = geometrySmith(N, V, L, roughness);
        const float3 F = fresnelSchlick(max(dot(H, V), 0.0f), F0);

        const float3 kD = (make_float3(1.0f) - F) * (1.0f - albedoMetallic.w);

        const float3 numerator = NDF * G * F;
        const float NdotL = max(dot(N, L), 0.0f);
        const float denominator = 4.0f * max(dot(N, V), 0.0f) * NdotL;
        const float3 specular = numerator / max(denominator, 0.001f);

        Lo += (kD * albedo / M_PIf + specular) * radiance * NdotL;
    }

    float3 ambient = make_float3(0.03f) * albedo /* * ao*/;
    if (use_envmap)
    {
        const float NdotV = dot(N, V);
        const float3 F =
            fresnelSchlickRoughness(max(NdotV, 0.0f), F0, roughness);
        const float3 kD = (make_float3(1.0f) - F) * (1.0f - albedoMetallic.w);

        const float3 irradiance = make_float3(
            rtTexCubemap<float4>(envmap_irradiance, N.x, N.y, N.z));
        const float3 diffuse = irradiance * albedo;

        // sample both the pre-filter map and the BRDF lut and combine them
        // together as per the Split-Sum approximation to get the IBL specular
        // part.
        const float3 reflectV = reflect(-V, N);
        const float3 prefilteredColor = make_float3(
            rtTexCubemapLod<float4>(envmap_radiance, reflectV.x, reflectV.y, reflectV.z,
                               roughness * float(radianceLODs)));
        const float2 brdf = make_float2(
            rtTex2D<float4>(envmap_brdf_lut, max(NdotV, 0.0), roughness));
        const float3 specular = prefilteredColor * (F * brdf.x + brdf.y);

        ambient = (kD * diffuse + specular) /* * ao*/;
    }

    const float3 color = ambient + Lo;
    prd.result = linearToSRGB(tonemap(color));
}

RT_PROGRAM void any_hit_shadow()
{
    rtTerminateRay();
}

RT_PROGRAM void closest_hit_radiance()
{
    shade();
}
