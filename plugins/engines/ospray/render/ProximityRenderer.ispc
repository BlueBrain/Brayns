/* Copyright (c) 2015-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of Brayns <https://github.com/BlueBrain/Brayns>
 *
 * Based on OSPRay implementation
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

#include "ExtendedOBJMaterial.ih"

#include <ospray/common/Model.ih>
#include <ospray/lights/Light.ih>
#include <ospray/render/Renderer.ih>

// Brayns
#include <plugins/engines/ospray/render/utils/RandomGenerator.ih>

#define ALPHA_THRESHOLD (.05f)

#define MATERIAL_SIMULATION 0
#define MATERIAL_SKYBOX 1

struct ProximityRenderer {
    Renderer super;
    const uniform Light *uniform *uniform lights;
    uint32 numLights;
    const uniform ExtendedOBJMaterial *uniform *uniform materials;
    uint32 numMaterials;
    vec3f bgColor;
    vec3f nearColor;
    vec3f farColor;
    float detectionDistance;
    bool detectionOnDifferentMaterial;
    bool electronShadingEnabled;
    int randomNumber;
    float timestamp;
    int spp;
};

/*
________________________________________________________________________________

Skybox mapping
________________________________________________________________________________
*/
vec4f skyboxMapping(
    const uniform ProximityRenderer* uniform self,
    Ray& ray )
{
    vec4f result = make_vec4f( self->bgColor );
    if( self->numMaterials <= MATERIAL_SKYBOX )
        return result;

    const float radius = 10000.f;
    const uniform ExtendedOBJMaterial *uniform mat =
        self->materials[MATERIAL_SKYBOX];
    if( !mat->map_Kd )
        return result;

    // solve the equation sphere-ray to find the intersections
    vec3f dir = ray.dir;

    float a = 2.f * dot( dir, dir );
    float b = 2.f * dot( ray.org, dir );
    float c = dot( ray.org, ray.org )- radius * radius;
    float d = b * b - 2.f * a * c;

    if(  d <= 0.f || a == 0.f)
        return result;

    float r = sqrt( d );
    float t1 = ( -b - r ) / a;
    float t2 = ( -b + r ) / a;

    if( t1 <= self->super.epsilon && t2<=self->super.epsilon )
        return result; // both intersections are behind the ray origin

    float t = 0.f;
    if( t1 <= self->super.epsilon )
        t = t2;
    else
        if( t2 <= self->super.epsilon )
            t = t1;
        else
            t = ( t1 < t2 ) ? t1 : t2;

    if( t<self->super.epsilon)
        return result; // Too close to intersection

    vec3f intersection = normalize(ray.org+t*dir);

    // Intersection found, now get skybox color
    vec2f st =
    {
        (( atan2( intersection.x, intersection.z ) / PI ) + 1.f ) * .5f,
        -( asin( intersection.y ) / PI ) + .5f
    };

    return get4f( mat->map_Kd, st );
}

inline vec3f ProximityRenderer_shadeRay(
        const uniform ProximityRenderer* uniform self,
        varying ScreenSample& sample)
{
    Ray ray = sample.ray;

    vec3f color = make_vec3f( 0.f );
    float path_opacity = 1.f;
    vec3f colorKs = make_vec3f( 0.f );

    traceRay( self->super.model, ray );

    if( ray.geomID < 0 )
    {
        // No intersection. Return skybox color
        color = color +
            path_opacity * make_vec3f( skyboxMapping( self, ray ));
        sample.z = 1;
        sample.alpha = 0.f;
        return self->bgColor + color + colorKs;
    }

    sample.alpha = 1.f;
    sample.z = log( ray.t ) / 10.f;

    DifferentialGeometry dg;
    postIntersect(
                self->super.model,
                dg,
                ray,
                DG_NG|DG_NS|DG_NORMALIZE|DG_FACEFORWARD|DG_MATERIALID|
                DG_COLOR|DG_TEXCOORD);

    uniform Material *material = dg.material;
    uniform ExtendedOBJMaterial *objMaterial =
            (uniform ExtendedOBJMaterial *)material;

    float emission = 0.f;
    float opacity = 1.f;
    float Ns = 0.f;
    vec3f Kd = make_vec3f( 0.0f, 0.0f, 0.0f );
    vec3f Ks = make_vec3f( 0.f );
    vec3f normal = dg.Ns;
    const vec3f P = dg.P + self->super.epsilon * dg.Ng;

    if( !objMaterial )
    {
        opacity = dg.color.w;
        Kd = make_vec3f( dg.color );
    }
    else
    {
        foreach_unique( mat in objMaterial )
        {
            emission = mat->a;
            opacity = mat->d * get1f( mat->map_d, dg.st, 1.f );
            Kd = mat->Kd * make_vec3f( dg.color );
        }
    }

    const float local_opacity = path_opacity * abs( opacity );

    if( local_opacity > 0.01f )
    {
        const vec3f R = ray.dir - ( 2.f * dot( ray.dir, normal )) * normal;

        //calculate shading for all lights
        for( uniform int i = 0; self->lights && i < self->numLights; i++ )
        {
            const uniform Light* uniform l = self->lights[i];

            const vec2f s = make_vec2f( 0.f );
            const Light_SampleRes light = l->sample( l, dg, s );
            float radiance = 1.f;
            vec3f L = light.dir;
            float dist;

            if(emission != 0 )
                radiance = emission;
            else
                radiance = reduce_max( light.weight );

            if( radiance > 0.f )
            {
                // Any potential contribution?
                float cosNL = 0.0;
                float cosLR = 0.0;
                if( self->electronShadingEnabled )
                {
                    // Electron shading
                    vec3f me = normalize( ray.org - P );
                    cosNL = 0.2f - 0.2f * max( 0.f, dot( me, normal ));
                }
                else
                {
                    cosNL = max( 0, dot( L, normal ));
                    cosLR = max( 0, dot( L, R ));
                }

                const vec3f unshaded_light_contrib =
                        local_opacity * Kd * cosNL * radiance;
                // Specular
                colorKs = max( colorKs, Ks * powf( cosLR, Ns ));

                color = color + unshaded_light_contrib;
            }
        }

        if (self->detectionDistance>0.f && ray.t<self->detectionDistance*1000.f )
        {
            // Generate random ray and trace it
            vec3f ao_dir = getRandomVector( sample, normal, self->randomNumber );

            if( dot( ao_dir, normal ) < 0.f )
                ao_dir = ao_dir * -1.f;
            Ray ao_ray = ray;
            ao_ray.org = P;
            ao_ray.dir = ao_dir;
            float t_max = self->detectionDistance;
            ao_ray.t0 = self->detectionDistance * 0.1f;
            ao_ray.t = t_max;

            traceRay( self->super.model, ao_ray );
            if( ao_ray.t != t_max )
            {
                // Intersection detected
                postIntersect( self->super.model, dg, ao_ray, DG_MATERIALID );

                bool test = true;
                if( self->detectionOnDifferentMaterial )
                    test =  material != dg.material;

                if( test )
                {
                    if( ao_ray.t < t_max * 0.2f )
                        color = self->nearColor * opacity / ao_ray.t;
                    else
                        color = self->farColor * opacity / ao_ray.t;
                }
            }
        }
    }

    return color;
}

void ProximityRenderer_renderSample(
    uniform Renderer* uniform _self,
    void * uniform perFrameData,
    varying ScreenSample& sample)
{
    uniform ProximityRenderer* uniform self =
            ( uniform ProximityRenderer* uniform )_self;
    sample.ray.time = self->timestamp;
    sample.rgb = ProximityRenderer_shadeRay( self, sample );
}


// Exports (called from C++)
export void *uniform ProximityRenderer_create( void* uniform cppE )
{
    uniform ProximityRenderer* uniform self =
            uniform new uniform ProximityRenderer;

    Renderer_Constructor( &self->super, cppE );
    self->super.renderSample = ProximityRenderer_renderSample;
    return self;
}

export void ProximityRenderer_set(
        void *uniform _self,
        const uniform vec3f& bgColor,
        const uniform vec3f& nearColor,
        const uniform vec3f& farColor,
        const uniform float& detectionDistance,
        const uniform bool& detectionOnDifferentMaterial,
        const uniform int& randomNumber,
        const uniform float& timestamp,
        const uniform int& spp,
        const uniform bool& electronShadingEnabled,
        void** uniform lights,
        uniform int32 numLights,
        void** uniform materials,
        uniform int32 numMaterials )
{
    uniform ProximityRenderer *uniform self =
            ( uniform ProximityRenderer* uniform )_self;

    self->bgColor = bgColor;
    self->nearColor = nearColor;
    self->farColor = farColor;
    self->detectionDistance = detectionDistance;
    self->detectionOnDifferentMaterial = detectionOnDifferentMaterial;
    self->randomNumber = randomNumber;
    self->timestamp = timestamp;
    self->spp = spp;
    self->electronShadingEnabled = electronShadingEnabled;

    self->lights = ( const uniform Light* uniform* uniform )lights;
    self->numLights = numLights;

    self->materials =
        ( const uniform ExtendedOBJMaterial* uniform* uniform )materials;
    self->numMaterials = numMaterials;
}
