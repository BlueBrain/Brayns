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

// clang-format off

#include "../../CommonStructs.h"
#include "../Helpers.h"
#include <optix_world.h>

using namespace optix;

// Scene
rtDeclareVariable(optix::Ray, ray, rtCurrentRay, );
rtDeclareVariable(PerRayData_radiance, prd, rtPayload, );
rtDeclareVariable(float, t_hit, rtIntersectionDistance, );
rtDeclareVariable(float, currentTime, , );

// Material attributes

rtDeclareVariable(float3, geometric_normal, attribute geometric_normal, );
rtDeclareVariable(float3, shading_normal, attribute shading_normal, );

// Textures
rtDeclareVariable(int, albedoMetallic_map, , );
rtDeclareVariable(int, normalRoughness_map, , );
rtDeclareVariable(int, aoEmissive_map, , );
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

rtDeclareVariable(float3, ddxWPos, attribute ddxWPos, );
rtDeclareVariable(float3, ddyWPos, attribute ddyWPos, );

rtDeclareVariable(uint, use_envmap, , );
rtDeclareVariable(int, envmap_radiance, , );
rtDeclareVariable(int, envmap_irradiance, , );
rtDeclareVariable(int, envmap_brdf_lut, , );
rtDeclareVariable(uint, radianceLODs, , );

static __device__ inline optix::Matrix3x3 cotangentFrame(float3 N, float3 ddxwpos, float3 ddywpos, float2 ddxuv, float2 ddyuv)
{
     // solve the linear system
    float3 dp2perp = cross( ddywpos, N );
    float3 dp1perp = cross( N, ddxwpos );
    float3 T = dp2perp * ddxuv.x + dp1perp * ddyuv.x;
    float3 B = dp2perp * ddxuv.y + dp1perp * ddyuv.y;

    // construct a scale-invariant frame
    float invmax = rsqrt(max(dot(T,T), dot(B,B)));

    optix::Matrix3x3 TBN;
    TBN.setCol(0, T * invmax);
    TBN.setCol(1, B * invmax);
    TBN.setCol(2, N);
    return TBN;
}

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

    const float3 WorldPos = ray.origin + t_hit * ray.direction;
    const float3 V = -ray.direction;

    // ******* triplanar ********
    float3 triblend = N * N;
    triblend = triblend / (triblend.x + triblend.y + triblend.z);

    const float planeSizeRatio = /*0.11f*/ 12.0f; // must be parameter
    const float3 uv = planeSizeRatio * WorldPos;
    const float3 ddxuv = planeSizeRatio * ddxWPos;
    const float3 ddyuv = planeSizeRatio * ddyWPos; 

    const float2 ddxuvX = make_float2(ddxuv.z, ddxuv.y);
    const float2 ddxuvY = make_float2(ddxuv.x, ddxuv.z);
    const float2 ddxuvZ = make_float2(ddxuv.x, ddxuv.y);

    const float2 ddyuvX = make_float2(ddyuv.z, ddyuv.y);
    const float2 ddyuvY = make_float2(ddyuv.x, ddyuv.z);
    const float2 ddyuvZ = make_float2(ddyuv.x, ddyuv.y); 

    const float4 albedoMetallicX = SRGBtoLinear(rtTex2DGrad<float4>(albedoMetallic_map, uv.z, uv.y, ddxuvX, ddyuvX)); 
    const float4 albedoMetallicY = SRGBtoLinear(rtTex2DGrad<float4>(albedoMetallic_map, uv.x, uv.z, ddxuvY, ddyuvY));
    const float4 albedoMetallicZ = SRGBtoLinear(rtTex2DGrad<float4>(albedoMetallic_map, uv.x, uv.y, ddxuvZ, ddyuvZ));
    const float4 albedoMetallic = triblend.y * albedoMetallicY + triblend.x * albedoMetallicX + triblend.z * albedoMetallicZ;
    const float3 albedo = make_float3(albedoMetallic);

    const float4 aoEmissiveX = rtTex2DGrad<float4>(aoEmissive_map, uv.z, uv.y, ddxuvX, ddyuvX);
    const float4 aoEmissiveY = rtTex2DGrad<float4>(aoEmissive_map, uv.x, uv.z, ddxuvY, ddyuvY);
    const float4 aoEmissiveZ = rtTex2DGrad<float4>(aoEmissive_map, uv.x, uv.y, ddxuvZ, ddyuvZ);
    const float4 aoEmissive = triblend.y * aoEmissiveY + triblend.x * aoEmissiveX + triblend.z * aoEmissiveZ;
    const float ao = aoEmissive.x;
    const float emissive = aoEmissive.y;    

    const float4 normalRoughnessX = rtTex2DGrad<float4>(normalRoughness_map, uv.z, uv.y, ddxuvX, ddyuvX );
    const float4 normalRoughnessY = rtTex2DGrad<float4>(normalRoughness_map, uv.x, uv.z, ddxuvY, ddyuvY );
    const float4 normalRoughnessZ = rtTex2DGrad<float4>(normalRoughness_map, uv.x, uv.y, ddxuvZ, ddyuvZ );

    float3 normalX = 2.0f * make_float3(normalRoughnessX) - 1.0f;
    float3 normalY = 2.0f * make_float3(normalRoughnessY) - 1.0f;
    float3 normalZ = 2.0f * make_float3(normalRoughnessZ) - 1.0f;

    /*** TANGENT SPACE MATRIX TECHNIQUE ***/

    // Calculate the cotangent frame for each plane
    const optix::Matrix3x3 tbnX = cotangentFrame(N, ddxWPos, ddyWPos, ddxuvX, ddyuvX );
    const optix::Matrix3x3 tbnY = cotangentFrame(N, ddxWPos, ddyWPos, ddxuvY, ddyuvY );
    const optix::Matrix3x3 tbnZ = cotangentFrame(N, ddxWPos, ddyWPos, ddxuvZ, ddyuvZ );

    // Apply cotangent frame and triblend normals
    N = normalize(tbnX * normalX * triblend.x + tbnY * normalY * triblend.y + tbnZ * normalZ * triblend.z);
   
    /*** TANGENT SPACE MATRIX TECHNIQUE ***/

    const float roughnessX = normalRoughnessX.w;
    const float roughnessY = normalRoughnessY.w;
    const float roughnessZ = normalRoughnessZ.w;
    const float roughness = triblend.y * roughnessY + triblend.x * roughnessX + triblend.z * roughnessZ;

    const float3 F0 = lerp(make_float3(0.04f), albedo, albedoMetallic.w);

    float3 Lo = make_float3(0.0f);
    unsigned int num_lights =  lights.size();
    for (int i = 0; i < num_lights; ++i)
    {
        // per-light radiance
        BasicLight light = lights[i];
        light.pos = make_float3(5.0f, 10.0f, 15.0f);
        const float3 L = normalize(light.pos - WorldPos);
        const float3 H = normalize(V + L);
        const float attenuation = calculateAttenuation(WorldPos, light.pos);
        const float3 radiance =
            light.color * attenuation * 45.0f;

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

    float3 ambient = make_float3(0.005f) * albedo * ao;
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

        ambient = (kD * diffuse + specular) * ao;
    }
    const int size = 28;
    float3 colormap[size] = {make_float3(0.0f, 0.0f, 1.0f), make_float3(0.0f, 0.0f, 1.0f), make_float3(0.0f, 0.0f, 1.0f), make_float3(0.0f, 0.0f, 1.0f),
                             make_float3(0.0f, 0.0f, 1.0f), make_float3(0.0f, 0.0f, 1.0f), make_float3(0.0f, 0.0f, 1.0f), make_float3(0.0f, 0.0f, 1.0f),
                             make_float3(0.0f, 0.0f, 1.0f), make_float3(0.0f, 0.0f, 1.0f), make_float3(0.0f, 0.0f, 1.0f), make_float3(0.0f, 0.0f, 1.0f),
                             make_float3(0.0f, 0.0f, 1.0f), make_float3(0.1f, 0.0f, 1.0f), make_float3(0.2f, 0.0f, 1.0f), make_float3(0.4f, 0.0f, 1.0f),
                             make_float3(0.75f, 0.0f, 1.0f), make_float3(1.0f, 0.0f, 0.9f), make_float3(1.0f, 0.0f, 0.8f), make_float3(1.0f, 0.0f, 0.6f),
                             make_float3(1.0f, 0.0f, 0.4f), make_float3(1.0f, 0.0f, 0.2f), make_float3(1.0f, 0.0f, 0.2f), make_float3(1.0f, 0.0f, 0.0f),
                             make_float3(1.0f, 0.0f, 0.0f), make_float3(1.0f, 0.0f, 0.0f), make_float3(1.0f, 0.0f, 0.0f), make_float3(1.0f, 0.0f, 0.0f)};
    float powermap[size] = {1.0f, 1.0f, 1.0f, 1.0f,
                            1.0f, 1.0f, 1.0f, 1.0f,
                            1.0f, 1.0f, 1.0f, 1.0f,
                            1.0f, 1.0f, 1.0f, 1.0f,
                            1.125f, 1.25f, 1.5f, 2.0f,
                            3.0f, 5.0f, 8.0f, 13.0f, 
                            21.0f, 34.0f, 55.0f, 89.0f};
    float period = 15.0f;
    float loopedTime = fmod(currentTime, period);
    float indexfloat = (size - 1) * loopedTime / period;
    int index = indexfloat;
    float frac = indexfloat - index;
    float3 emmissiveColor = ((1.0f - frac) * colormap[index] + frac * colormap[index + 1]) * ((1.0f - frac) * powermap[index] + frac * powermap[index + 1]);
    

    const float3 color = ambient + Lo + emissive * emmissiveColor;
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

// clang-format on
