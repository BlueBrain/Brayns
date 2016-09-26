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
 * Added support for electron shading, shadows, soft shadows and ambient occlusion
 */

#include <optix_world.h>
#include "../CommonStructs.h"
#include "../Helpers.h"
#include "../Random.h"

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


rtDeclareVariable(int,               max_depth, , );
rtBuffer<BasicLight>                 lights;
rtDeclareVariable(float3,            ambient_light_color, , );
rtDeclareVariable(unsigned int,      radiance_ray_type, , );
rtDeclareVariable(unsigned int,      shadow_ray_type, , );
rtDeclareVariable(float,             scene_epsilon, , );
rtDeclareVariable(rtObject,          top_object, , );
rtDeclareVariable(rtObject,          top_shadower, , );
rtDeclareVariable(float3,            eye, , );
rtDeclareVariable(uint,              shading_enabled, , );
rtDeclareVariable(uint,              shadows_enabled, , );
rtDeclareVariable(uint,              soft_shadows_enabled, , );
rtDeclareVariable(float,             ambient_occlusion_strength, , );
rtDeclareVariable(float4,            jitter4, , );

rtBuffer<uchar4, 2>              output_buffer;

rtDeclareVariable(optix::Ray, ray, rtCurrentRay, );
rtDeclareVariable(float, t_hit, rtIntersectionDistance, );
rtDeclareVariable(PerRayData_radiance, prd, rtPayload, );
rtDeclareVariable(PerRayData_shadow,   prd_shadow, rtPayload, );
rtDeclareVariable(unsigned int,  frame, , );
rtDeclareVariable(uint2,         launch_index, rtLaunchIndex, );

static __device__ void phongShadowed()
{
    // this material is opaque, so it fully attenuates all shadow rays
    prd_shadow.attenuation = optix::make_float3(0.0f);
    rtTerminateRay();
}

static __device__ void electronShade(
    float3 Kd,
    float3 Ka,
    float3 normal )
{
    float3 hit_point = ray.origin + t_hit * ray.direction;

    // ambient contribution
    float3 result = Ka * ambient_light_color;
    result += Kd * (1.f - abs(optix::dot( optix::normalize(hit_point-eye), normal)));

    // pass the color back up the tree
    prd.result = result;
}

static __device__ void phongShade(
    float3 p_Kd,
    float3 p_Ka,
    float3 p_Ks,
    float3 p_Kr,
    float  p_phong_exp,
    float3 p_normal )
{
    float3 hit_point = ray.origin + t_hit * ray.direction;
    optix::size_t2 screen = output_buffer.size();
    unsigned int seed = tea<16>( screen.x*launch_index.y + launch_index.x, frame );

    // ambient contribution
    float3 result = p_Ka * ambient_light_color;

    if( shading_enabled == 0 )
        result = p_Kd;
    else
    {
        // compute direct lighting
        unsigned int num_lights = lights.size();
        for(int i = 0; i < num_lights; ++i)
        {
            BasicLight light = lights[i];

            float3 L;
            float Ldist = RT_DEFAULT_MAX;
            if( light.type == 0 )
            {
                // Point light
                float3 pos = light.pos;
                if( soft_shadows_enabled == 1 )
                    // Soft shadows
                    pos += 5.f * make_float3(
                        rnd( seed ) - 0.5f, rnd( seed ) - 0.5f, rnd( seed ) - 0.5f );
                L = optix::normalize( pos - hit_point );
                Ldist = optix::length( pos - hit_point );
            }
            else
            {
                // Directional light
                L = -light.dir;
                if( soft_shadows_enabled == 1 )
                    // Soft shadows
                    L += 0.05f * make_float3(
                        rnd( seed ) - 0.5f, rnd( seed ) - 0.5f, rnd( seed ) - 0.5f );
                L = optix::normalize( L );
            }
            float nDl = optix::dot( p_normal, L );

            // Shadows
            float3 light_attenuation = make_float3(1.f);
            if( shadows_enabled )
            {
                light_attenuation = make_float3(static_cast<float>( nDl > 0.0f ));
                if ( nDl > 0.0f && light.casts_shadow )
                {
                    PerRayData_shadow shadow_prd;
                    shadow_prd.attenuation = make_float3(1.0f);
                    optix::Ray shadow_ray = optix::make_Ray(
                        hit_point, L, shadow_ray_type, scene_epsilon, Ldist );
                    rtTrace(top_shadower, shadow_ray, shadow_prd);
                    light_attenuation = shadow_prd.attenuation;
                }
            }

            // If not completely shadowed, light the hit point
            if( fmaxf(light_attenuation) > 0.0f )
            {
                float3 Lc = light.color * light_attenuation;

                result += p_Kd * nDl * Lc;

                float3 H = optix::normalize(L - ray.direction);
                float nDh = optix::dot( p_normal, H );
                if(nDh > 0)
                {
                    float power = pow(nDh, p_phong_exp);
                    result += p_Ks * power * Lc;
                }
            }
        }
    }

    if( fmaxf( p_Kr ) > 0 )
    {
        // ray tree attenuation
        PerRayData_radiance new_prd;
        new_prd.importance = prd.importance * optix::luminance( p_Kr );
        new_prd.depth = prd.depth + 1;

        // reflection ray
        if( new_prd.importance >= 0.01f && new_prd.depth <= max_depth)
        {
            float3 R = optix::reflect( ray.direction, p_normal );
            optix::Ray refl_ray = optix::make_Ray( hit_point, R, radiance_ray_type, scene_epsilon, RT_DEFAULT_MAX );
            rtTrace(top_object, refl_ray, new_prd);
            result += p_Kr * new_prd.result;
        }
    }

    // Ambient occlusion
    if( ambient_occlusion_strength > 0.f )
    {
        PerRayData_shadow aa_prd;
        aa_prd.attenuation = make_float3(1.f);

        float3 aa_normal = optix::normalize( make_float3(rnd( seed ) - 0.5f, rnd( seed ) - 0.5f, rnd( seed ) - 0.5f ));
        if( optix::dot(aa_normal, p_normal) < 0.f )
            aa_normal = -aa_normal;
        optix::Ray aa_ray = optix::make_Ray( hit_point, aa_normal, shadow_ray_type, scene_epsilon, RT_DEFAULT_MAX );
        rtTrace(top_shadower, aa_ray, aa_prd);
        result *= 1.f - ambient_occlusion_strength * (1.f - aa_prd.attenuation);
    }

    // pass the color back up the tree
    prd.result = result;
}
