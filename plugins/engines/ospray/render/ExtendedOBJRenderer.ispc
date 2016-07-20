/* Copyright (c) 2015-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * Based on OSPRay implementation
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

// ospray
#include <ospray/fb/FrameBuffer.ih>
#include <ospray/camera/PerspectiveCamera.ih>
#include <ospray/common/Model.ih>
#include <ospray/texture/Texture2D.ih>
#include <ospray/lights/Light.ih>
#include <ospray/render/Renderer.ih>

// obj renderer
#include <ospray/render/scivis/SciVisMaterial.ih>
#include "ExtendedOBJMaterial.ih"

// Brayns
#include <plugins/engines/ospray/render/utils/RandomGenerator.ih>

#define ALPHA_THRESHOLD (.05f)

#define MATERIAL_SIMULATION 0
#define MATERIAL_SKYBOX 1


struct ExtendedOBJRenderer
{
    Renderer super;

    const uniform Light *uniform *uniform lights;
    uint32 numLights;
    const uniform ExtendedOBJMaterial *uniform *uniform materials;
    uint32 numMaterials;
    vec3f bgColor;
    bool shadowsEnabled;
    bool shadingEnabled;
    bool softShadowsEnabled;
    float ambientOcclusionStrength;
    bool electronShadingEnabled;
    int randomNumber;
    bool moving;
    float timestamp;
    int spp;
};

/*
________________________________________________________________________________

Skybox mapping
________________________________________________________________________________
*/
vec4f skyboxMapping(
    const uniform ExtendedOBJRenderer* uniform self,
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

inline float lightAlpha(
    Ray& ray,
    uniform Model* uniform model,
    const float weight,
    const uniform float epsilon )
{
    float alpha = 1.f;
    int max_depth = 8;
    const float org_t_max = ray.t;

    while( true )
    {
        traceRay( model, ray );

        if( ray.geomID < 0 )
            return alpha;

        DifferentialGeometry dg;
        postIntersect( model, dg, ray, DG_MATERIALID | DG_TEXCOORD | DG_COLOR );

        uniform ExtendedOBJMaterial* objMaterial =
                ( uniform ExtendedOBJMaterial* )dg.material;

        float material_opacity = 1.f;

        if( objMaterial == 0 )
        {
            material_opacity = 1.0 - dg.color.w;
        }
        else
        {
            foreach_unique( mat in objMaterial )
            {
                material_opacity = mat->d;
                if( mat->map_d != 0 )
                {
                    vec4f d_map = mat->map_d->get( mat->map_d, dg.st );
                    material_opacity = d_map.x;
                }
                else if( mat->map_Kd != 0 )
                {
                    vec4f kd_map = mat->map_Kd->get( mat->map_Kd, dg.st );
                    material_opacity = 1.f - kd_map.w;
                }
            }
        }

        alpha -= material_opacity;

        if( alpha * weight < ALPHA_THRESHOLD )
            return alpha;

        max_depth--;
        if( max_depth <= 0 )
            return alpha;

        ray.t0 = ray.t + epsilon;
        ray.t = org_t_max;
        ray.primID = -1;
        ray.geomID = -1;
        ray.instID = -1;
    }
}

inline bool launchRandomRay(
    const uniform ExtendedOBJRenderer* uniform self,
    Ray& ray,
    varying ScreenSample& sample,
    const vec3f& origin,
    const vec3f& normal,
    DifferentialGeometry& dg )
{
    vec3f ao_dir = getRandomVector( sample, normal, self->randomNumber );

    if( dot( ao_dir, normal ) < 0.f )
        ao_dir = ao_dir * -1.f;
    Ray ao_ray = ray;
    ao_ray.org = origin;
    ao_ray.dir = ao_dir;
    ao_ray.t0 = self->super.epsilon * 100.f;
    const float maxDistance = self->super.epsilon * 100000.f;
    ao_ray.t = maxDistance;

    traceRay( self->super.model, ao_ray );
    if( ao_ray.t != maxDistance )
    {
        postIntersect(
            self->super.model, dg,
            ao_ray,
            DG_NG|DG_NS|DG_NORMALIZE|DG_FACEFORWARD|DG_MATERIALID|
            DG_COLOR|DG_TEXCOORD);
        return true;
    }
    return false;
}

inline vec3f ExtendedOBJRenderer_shadeRay(
    const uniform ExtendedOBJRenderer* uniform self,
    varying ScreenSample& sample)
{
    Ray ray = sample.ray;
    vec3f color = make_vec3f( 0.f );
    float path_opacity = 1.f;
    int max_depth = self->moving ? 2 : 10;
    uniform int depth = 0;
    vec3f colorKs = make_vec3f( 0.f );
    sample.alpha = 1.f;

    while( depth < max_depth )
    {
        traceRay( self->super.model, ray );

        if( ray.geomID < 0 )
        {
            // No intersection. Return skybox color
            color = color +
                path_opacity * make_vec3f( skyboxMapping( self, ray ));
            sample.z = 1;
            if( depth == 0 )
                sample.alpha = 0.f;
            return color + colorKs;
        }

        if( depth == 0 )
            sample.z = log( ray.t ) / 10.f;

        vec3f local_shade_color = make_vec3f( 0.f );

        DifferentialGeometry dg;
        postIntersect(
            self->super.model,
            dg,
            ray,
            DG_NG|DG_NS|DG_NORMALIZE|DG_FACEFORWARD|DG_MATERIALID|
            DG_COLOR|DG_TEXCOORD);

        uniform Material* material = dg.material;
        uniform ExtendedOBJMaterial* objMaterial =
                ( uniform ExtendedOBJMaterial* ) material;

        float lightEmission = 0.f;
        float refraction = 1.f;
        float reflection = 0.f;
        float opacity = 1.f;
        float Ns = 0.f;
        vec3f Kd = make_vec3f( 0.f, 0.f, 0.f );
        vec3f Ks = make_vec3f( 0.f );
        vec3f normal = dg.Ns;

        if( !objMaterial )
        {
            opacity = dg.color.w;
            Kd = make_vec3f( dg.color );
        }
        else
        {
            foreach_unique( mat in objMaterial )
            {
                lightEmission = mat->a;
                refraction = mat->refraction;
                opacity = mat->d * get1f( mat->map_d, dg.st, 1.f );
                Kd = mat->Kd * make_vec3f( dg.color );
                if( mat->map_Kd )
                {
                    vec4f Kd_from_map = get4f( mat->map_Kd, dg.st );
                    Kd = Kd * make_vec3f( Kd_from_map );
                    opacity *= Kd_from_map.w;
                }
                Ks = max(Ks, mat->Ks * get3f(
                             mat->map_Ks, dg.st, make_vec3f( 1.f )));
                Ns = max(Ns, mat->Ns * get1f(
                             mat->map_Ns, dg.st, 1.f ));

                if( mat->map_Normal )
                {
                    vec3f n = get3f( mat->map_Normal,
                        dg.st, make_vec3f( 0.f )) - 0.5f;
                    normal.x -= n.x;
                    normal.y -= n.y;
                }

                reflection = mat->reflection;
                if( mat->map_reflection )
                {
                    vec3f n = get3f( mat->map_reflection,
                        dg.st, make_vec3f( 0.f ));
                    reflection *= ( n.x + n.y + n.z ) / 3.f;
                }
            }
        }
        const float local_opacity = path_opacity * abs( opacity );

        max_depth -= 1;
        if( max_depth <= 0 )
            return color;

        const vec3f P = dg.P + self->super.epsilon * dg.Ng;
        float light_alpha = 1.f;
        if( local_opacity > 0.01f )
        {
            // Worth shading?
            const vec3f R = ray.dir - 2.f * dot( ray.dir, normal ) * normal;

            // Calculate shading for all lights
            for( uniform int i = 0; self->lights && i < self->numLights; ++i )
            {
                const uniform Light* uniform l = self->lights[ i ];

                const vec2f s = make_vec2f( 0.f );
                const Light_SampleRes light = l->sample( l, dg, s );
                vec3f radiance = light.weight;
                vec3f L = light.dir;

                if( lightEmission != 0.f )
                    radiance = make_vec3f( lightEmission );

                if( reduce_max( radiance ) > 0.f )
                {
                    // Any potential contribution?
                    float cosNL = 0.f;
                    float cosLR = 0.f;
                    if( self->electronShadingEnabled )
                    {
                        // Electron shading
                        vec3f me = normalize( ray.org - P );
                        float el = max( 0.f, dot( me, normal ));
                        cosNL = 1.f - el;
                    }
                    else
                    {
                        cosNL = 1.f;
                        cosLR = 1.f;
                        if( lightEmission == 0.f && self->shadingEnabled )
                        {
                            cosNL = max( 0.f, dot( L, normal ));
                            cosLR = max( 0.f, dot( L, R ));
                        }
                    }

                    const vec3f unshaded_light_contrib =
                            local_opacity * Kd * cosNL * radiance;

                    // Specular
                    if( self->shadingEnabled )
                        colorKs = Ks * powf( cosLR, Ns );

                    if( self->shadowsEnabled && lightEmission == 0.f )
                    {
                        if( self->softShadowsEnabled )
                        {
                            // Soft shadows
                            const vec3f ss =
                                getRandomVector( sample, normal, self->randomNumber );
                            L = L + ss * 0.1f;
                        }

                        const float max_contrib =
                                reduce_max( unshaded_light_contrib );
                        if( max_contrib > 0.01f )
                        {
                            Ray shadowRay;
                            setRay( shadowRay, P, L );
                            shadowRay.t0 = self->super.epsilon;
                            shadowRay.time = sample.ray.time;
                            light_alpha = lightAlpha(
                                shadowRay,
                                self->super.model,
                                max_contrib,
                                self->super.epsilon );
                        }
                    }
                    local_shade_color = colorKs + local_shade_color +
                        unshaded_light_contrib;
                }
            }

            if( lightEmission == 0.f && self->ambientOcclusionStrength != 0.f )
            {
                // Ambient occulusion and light emission
                DifferentialGeometry geometry;
                if( launchRandomRay( self, ray, sample, P, normal, geometry ))
                {
                    material = geometry.material;
                    objMaterial = ( uniform ExtendedOBJMaterial* )material;
                    if( objMaterial )
                        foreach_unique( mat in objMaterial )
                        {
                            if( mat->a != 0.f )
                            {
                                vec3f Kd = mat->Kd;
                                if( mat->map_Kd )
                                    Kd = get3f( mat->map_Kd, dg.st );
                                vec3f Ka = make_vec3f( mat->a );
                                if( mat->map_a )
                                    Ka = get3f( mat->map_a, dg.st );
                                local_shade_color = local_shade_color +
                                    make_vec3f( Kd * Ka * opacity * mat->d );
                            }
                            else
                                local_shade_color = local_shade_color -
                                    make_vec3f( self->ambientOcclusionStrength *
                                        opacity * mat->d );
                        }
                    else
                        local_shade_color = local_shade_color -
                                make_vec3f(
                                    self->ambientOcclusionStrength *
                                    opacity);
                }
            }

            float weight = path_opacity * light_alpha;
            color = ( 1.f - weight ) * color +
                    weight * ( max( make_vec3f( 0.f ), local_shade_color ));

            if( opacity < 1.f )
            {
                vec3f refract = ray.dir;
                float eta = refraction;
                float c1 = -dot( ray.dir, normal );
                float cs2 = 1.f - eta * eta * ( 1.f - c1 * c1 );
                if( cs2 > 0.f )
                    refract = eta * ray.dir +
                        ( eta * c1 - sqrt( cs2 )) * normal;
                ray.dir = refract;
                ray.t0 = ray.t + self->super.epsilon;
                path_opacity = path_opacity - opacity;
                sample.alpha = max(0.f, 1.f - path_opacity);
            }
            else if( reflection > 0.f )
            {
                ray.dir = R;
                ray.t0 = self->super.epsilon;
                ray.org = P;
                path_opacity = path_opacity - abs( 1.f - reflection );
            }
        }

        if( path_opacity < 0.01f )
            //remaining contribution too low kill path
            return color;

        ray.t = infinity;
        ray.primID = -1;
        ray.geomID = -1;
        ray.instID = -1;
        ++depth;
    }

    return color;
}

void ExtendedOBJRenderer_renderSample(
    uniform Renderer* uniform _self,
    void * uniform perFrameData,
    varying ScreenSample& sample)
{
    uniform ExtendedOBJRenderer* uniform self =
            ( uniform ExtendedOBJRenderer* uniform )_self;
    sample.ray.time = self->timestamp;
    sample.rgb = ExtendedOBJRenderer_shadeRay( self, sample );
}

// Exports (called from C++)
export void* uniform ExtendedOBJRenderer_create( void* uniform cppE )
{
    uniform ExtendedOBJRenderer* uniform self =
            uniform new uniform ExtendedOBJRenderer;

    Renderer_Constructor( &self->super, cppE );
    self->super.renderSample = ExtendedOBJRenderer_renderSample;
    return self;
}

export void ExtendedOBJRenderer_set(
        void* uniform _self,
        const uniform vec3f& bgColor,
        const uniform bool& shadowsEnabled,
        const uniform bool& softShadowsEnabled,
        const uniform float& ambientOcclusionStrength,
        const uniform bool& shadingEnabled,
        const uniform int& randomNumber,
        const uniform bool& moving,
        const uniform float& timestamp,
        const uniform int& spp,
        const uniform bool& electronShadingEnabled,
        void** uniform lights,
        uniform int32 numLights,
        void** uniform materials,
        uniform int32 numMaterials )
{
    uniform ExtendedOBJRenderer* uniform self =
            ( uniform ExtendedOBJRenderer* uniform )_self;

    self->bgColor = bgColor;
    self->shadowsEnabled = shadowsEnabled && !moving;
    self->softShadowsEnabled = softShadowsEnabled && !moving;
    self->ambientOcclusionStrength = ambientOcclusionStrength;
    self->shadingEnabled = shadingEnabled && !moving;
    self->randomNumber = randomNumber;
    self->moving = moving;
    self->timestamp = timestamp;
    self->spp = spp;
    self->electronShadingEnabled = electronShadingEnabled || moving;

    self->lights =
        ( const uniform Light* uniform* uniform )lights;
    self->numLights = numLights;

    self->materials =
        ( const uniform ExtendedOBJMaterial* uniform* uniform )materials;
    self->numMaterials = numMaterials;
}
