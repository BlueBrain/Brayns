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

#include <optix.h>
#include <optixu/optixu_math_namespace.h>
#include "Phong.h"

using namespace optix;

// Material attributes
rtDeclareVariable(float3,       Ka, , );
rtDeclareVariable(float3,       Kd, , );
rtDeclareVariable(float3,       Ks, , );
rtDeclareVariable(float3,       Kr, , );
rtDeclareVariable(float3,       Ko, , );
rtDeclareVariable(float,        glossiness, , );
rtDeclareVariable(unsigned int, shading_mode, , );
rtDeclareVariable(float,        refraction_index, , );
rtDeclareVariable(float,        phong_exp, , );

rtDeclareVariable(float3, geometric_normal, attribute geometric_normal, ); 
rtDeclareVariable(float3, shading_normal, attribute shading_normal, ); 

// Textures
rtTextureSampler<float4, 2> diffuse_map;
rtDeclareVariable(float3, texcoord, attribute texcoord, );


RT_PROGRAM void any_hit_shadow()
{
    phongShadowed( Ko );
}


RT_PROGRAM void closest_hit_radiance()
{
    float3 world_shading_normal   = normalize( rtTransformNormal( RT_OBJECT_TO_WORLD, shading_normal ) );
    float3 world_geometric_normal = normalize( rtTransformNormal( RT_OBJECT_TO_WORLD, geometric_normal ) );

    float3 ffnormal = faceforward( world_shading_normal, -ray.direction, world_geometric_normal );
    phongShade( Kd, Ka, Ks, Kr, Ko, refraction_index, phong_exp, glossiness, shading_mode, ffnormal, ray.tmax );
}


RT_PROGRAM void closest_hit_radiance_textured()
{
    float3 world_shading_normal   = normalize( rtTransformNormal( RT_OBJECT_TO_WORLD, shading_normal ) );
    float3 world_geometric_normal = normalize( rtTransformNormal( RT_OBJECT_TO_WORLD, geometric_normal ) );

    float3 ffnormal = faceforward( world_shading_normal, -ray.direction, world_geometric_normal );

    const float3 Kd = make_float3( tex2D( diffuse_map, texcoord.x, texcoord.y ) );
    phongShade( Kd, Ka, Ks, Kr, Ko, refraction_index, phong_exp, glossiness, shading_mode, ffnormal, ray.tmax );
}
