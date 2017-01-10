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

// Brayns
#include <plugins/engines/ospray/ispc/render/utils/AbstractRenderer.ih>

struct ExtendedOBJRenderer
{
    AbstractRenderer abstract;
};

/**
    Renderer a pixel color according to a given location in the screen space.
    @param self Pointer to current renderer
    @param sample Screen sample containing information about the ray, and the
           location in the screen space.
*/
inline vec3f ExtendedOBJRenderer_shadeRay(
    const uniform ExtendedOBJRenderer* uniform self,
    varying ScreenSample& sample)
{
    Ray ray = sample.ray;
    varying float pathOpacity = 0.f;
    varying int depth = 0;
    varying float oldlocalRefraction = 1.f;

    varying float intersectionWeights[NB_MAX_REBOUNDS];
    varying vec3f intersectionColors[NB_MAX_REBOUNDS];
    varying vec3f specularColor = make_vec3f( 0.f );
    sample.z = 1.f;

    varying bool moreRebounds = true;
    while( moreRebounds && depth < NB_MAX_REBOUNDS )
    {
        intersectionWeights[depth] = 1.f;
        intersectionColors[depth] = make_vec3f( 0.f );
        traceRay( self->abstract.super.model, ray );

        if( ray.geomID < 0 )
        {
            // No intersection. Return skybox color
            intersectionColors[depth] =
                make_vec3f( skyboxMapping(
                    (Renderer *)self, ray, self->abstract.numMaterials, self->abstract.materials ));
            moreRebounds = false;
        }
        else
        {
            // Intersection found
            if( depth == 0 )
                sample.z = log( ray.t ) / 10.f;

            // Retreive information about the geometry, typically geometry ID,
            // normal to the surface, material ID, texture coordinates, etc.
            DifferentialGeometry dg;
            postIntersect(
                self->abstract.super.model, dg, ray,
                DG_NG|DG_NS|DG_NORMALIZE|DG_FACEFORWARD|DG_MATERIALID|
                DG_COLOR|DG_TEXCOORD);

            // Initialisation for the current intersection
            varying float localLightEmission = 0.f;
            varying float localRefraction = 1.f;
            varying float localReflection = 0.f;
            varying float localOpacity = 0.f;
            varying vec3f localDiffuseColor = make_vec3f( 0.f );
            varying vec3f localSpecularColor = make_vec3f( 0.f );
            varying float localSpecularExponent = 0.f;
            varying vec3f localNormal = dg.Ns;
            varying float localLightIntensity = 1.f;

            // Process material attributes
            const uniform Material* material = dg.material;
            const uniform ExtendedOBJMaterial* objMaterial =
                    ( const uniform ExtendedOBJMaterial* ) material;
            if( !objMaterial )
            {
                // Diffuse color
                localDiffuseColor = make_vec3f( dg.color );
                // if no material is defined, localOpacity is stored in the alpha
                // component of the RGBA color
                localOpacity = dg.color.w;
            }
            else
            {
                foreach_unique( mat in objMaterial )
                {
                    // Light emission strength
                    localLightEmission = mat->a;

                    // Refraction index
                    localRefraction = mat->refraction;

                    // localOpacity
                    localOpacity = mat->d * get1f( mat->map_d, dg.st, 1.f );

                    // Diffuse color
                    localDiffuseColor = mat->Kd * make_vec3f( dg.color );
                    if( mat->map_Kd )
                    {
                        vec4f localDiffuseColorFromMap = get4f( mat->map_Kd, dg.st );
                        localDiffuseColor =
                            localDiffuseColor * make_vec3f( localDiffuseColorFromMap );
                        localOpacity *= localDiffuseColorFromMap.w;
                    }

                    // Specular color
                    localSpecularColor = max(
                        localSpecularColor,
                        mat->Ks * get3f( mat->map_Ks, dg.st, make_vec3f( 1.f )));

                    // Specular exponent
                    localSpecularExponent = max(
                        localSpecularExponent,
                        mat->Ns * get1f( mat->map_Ns, dg.st, 1.f ));

                    // Normal mapping
                    if( mat->map_Normal )
                    {
                        vec3f n = get3f( mat->map_Normal, dg.st, make_vec3f( 0.f )) - 0.5f;
                        localNormal.x -= n.x;
                        localNormal.y -= n.y;
                    }

                    // localReflection index
                    localReflection = mat->reflection;
                    if( mat->map_reflection )
                    {
                        vec3f n = get3f( mat->map_reflection, dg.st, make_vec3f( 0.f ));
                        localReflection *= ( n.x + n.y + n.z ) / 3.f;
                    }
                }
            }

            // localShadedColor defines the color for the current intersected
            // surface, and for the current ray generation only. This value is
            // reset and updated for every rebound
            varying vec3f localShadedColor = make_vec3f(0.f);

            // Intersection point with geometry, slightly above the surface,
            // along the normal
            const varying vec3f intersection = dg.P + self->abstract.super.epsilon * dg.Ng;

            if( localOpacity > 0.01f )
            {
                varying vec3f indirectShadingColor = make_vec3f( 0.f );
                varying float indirectShadingIntensity = 0.f;
                const varying vec3f reflectedDirection =
                    ray.dir - 2.f * dot( ray.dir, localNormal ) * localNormal;

                if( localLightEmission == 0.f && self->abstract.ambientOcclusionStrength != 0.f )
                {
                    // Indirect illumination
                    DifferentialGeometry geometry;
                    indirectShading(
                        &(self->abstract), ray, sample, intersection, localNormal,
                        geometry, indirectShadingColor, indirectShadingIntensity );

                    indirectShadingColor =
                        indirectShadingColor * self->abstract.ambientOcclusionStrength;
                    indirectShadingIntensity *= self->abstract.ambientOcclusionStrength;

                    indirectShadingColor = indirectShadingColor * localOpacity;
                }

                if( self->abstract.numLights == 0 )
                {
                    // No lights defined
                    localLightIntensity = localLightEmission;
                    localLightIntensity = max( localLightIntensity, indirectShadingIntensity );
                    localShadedColor = localOpacity * ( localDiffuseColor + indirectShadingColor );
                }
                else if( self->abstract.electronShadingEnabled )
                {
                    // Electron Shading
                    localLightIntensity = 1.f;
                    const varying vec3f viewer =
                        normalize( ray.org - intersection );
                    const varying float el =
                        max( 0.f, dot( viewer, localNormal ));
                    varying float cosNL  = 1.f - el;
                    localShadedColor =
                        localOpacity * cosNL *
                        (localDiffuseColor + indirectShadingColor * indirectShadingIntensity);
                }
                else
                {
                    // Shading for all lights defined in the scene
                    for( uniform int i = 0; self->abstract.lights && i < self->abstract.numLights; ++i )
                    {
                        const uniform Light* uniform light = self->abstract.lights[ i ];
                        const varying vec2f s = make_vec2f( 0.5f );
                        const varying Light_SampleRes lightSample = light->sample( light, dg, s );
                        varying vec3f radiance = lightSample.weight + indirectShadingIntensity;
                        varying vec3f lightDirection = lightSample.dir;

                        if( reduce_max( radiance ) > 0.f )
                        {
                            varying float cosNL = 1.f;
                            varying float cosLR = 1.f;
                            // Phong + Blinn shading
                            cosNL = 1.f;
                            cosLR = 1.f;
                            if( localLightEmission == 0.f && self->abstract.shadingEnabled )
                            {
                                cosNL = min( 1.f,
                                    max( 0.f, indirectShadingIntensity ) +
                                    max( 0.f, dot( lightDirection, localNormal )));
                                cosLR = min( 1.f,
                                    max( 0.f, indirectShadingIntensity ) +
                                    max( 0.f, dot( lightDirection, reflectedDirection )));
                            }

                            // Diffuse color
                            const varying vec3f localUnshadedColor =
                                localOpacity * cosNL *
                                (localDiffuseColor + indirectShadingColor * indirectShadingIntensity);

                            // Specular color
                            if( self->abstract.shadingEnabled )
                                specularColor = max(
                                    specularColor,
                                    localSpecularColor * powf( cosLR, localSpecularExponent ));

                            // Process shadows
                            if( self->abstract.shadowsEnabled && localLightEmission == 0.f )
                                localLightIntensity = max( 0.f,
                                        indirectShadingIntensity + shadedLightIntensity(
                                            &(self->abstract), ray, sample,
                                            intersection, localNormal, lightDirection ));

                            // Add contribution of current light
                            localShadedColor = localShadedColor + localUnshadedColor;
                        }
                    }
                }

                // Store final color for current iteration
                intersectionColors[depth] = localShadedColor * localLightIntensity + specularColor;

                // Update cumulated path opacity
                pathOpacity += localOpacity;
                pathOpacity = min( 1.f, pathOpacity );

                // Prepare ray for next iteration
                if( localOpacity < 1.f )
                {
                    const float angle = dot( ray.dir, localNormal );
                    if( localRefraction == oldlocalRefraction )
                        if( angle < 0.f )
                            // Set localRefraction to default index if exiting volume
                            localRefraction = 1.f;

                    ray.t0 = ray.t + self->abstract.super.epsilon;
                    ray.dir = refractedVector(
                        ray.dir, localNormal, oldlocalRefraction, localRefraction );

                    intersectionWeights[depth] = 1.f - localOpacity;
                    oldlocalRefraction = localRefraction;
                }
                else if( localReflection > 0.f )
                {
                    ray.dir = reflectedDirection;
                    ray.t0 = self->abstract.super.epsilon;
                    ray.org = intersection;
                    intersectionWeights[depth] = localReflection;
                }
                else
                    moreRebounds = false;
            }
            ray.t = infinity;
            ray.primID = -1;
            ray.geomID = -1;
            ray.instID = -1;
        }
        ++depth;
    }

    // Back to front computation of final color according to colors and weights of each ray
    // generation
    sample.alpha =  pathOpacity;

    for( int i = depth - 2; i >= 0; --i )
        intersectionColors[i] =
            intersectionColors[i+1] * intersectionWeights[i] +
            intersectionColors[i] * ( 1.f - intersectionWeights[i] );

    return intersectionColors[0];
}

void ExtendedOBJRenderer_renderSample(
    uniform Renderer* uniform _self,
    void * uniform perFrameData,
    varying ScreenSample& sample)
{
    uniform ExtendedOBJRenderer* uniform self =
            ( uniform ExtendedOBJRenderer* uniform )_self;
    sample.ray.time = self->abstract.timestamp;
    sample.rgb = ExtendedOBJRenderer_shadeRay( self, sample );
}

// Exports (called from C++)
export void* uniform ExtendedOBJRenderer_create( void* uniform cppE )
{
    uniform ExtendedOBJRenderer* uniform self =
            uniform new uniform ExtendedOBJRenderer;
    Renderer_Constructor( &self->abstract.super, cppE );
    self->abstract.super.renderSample = ExtendedOBJRenderer_renderSample;
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
        const uniform float& timestamp,
        const uniform int& spp,
        const uniform bool& electronShadingEnabled,
        void** uniform lights,
        uniform int32 numLights,
        void** uniform materials,
        uniform int32 numMaterials )
{
    uniform ExtendedOBJRenderer* uniform self = ( uniform ExtendedOBJRenderer* uniform )_self;

    self->abstract.bgColor = bgColor;
    self->abstract.shadowsEnabled = shadowsEnabled;
    self->abstract.softShadowsEnabled = softShadowsEnabled;
    self->abstract.ambientOcclusionStrength = ambientOcclusionStrength;
    self->abstract.shadingEnabled = shadingEnabled;
    self->abstract.randomNumber = randomNumber;
    self->abstract.timestamp = timestamp;
    self->abstract.spp = spp;
    self->abstract.electronShadingEnabled = electronShadingEnabled;

    self->abstract.lights = ( const uniform Light* uniform* uniform )lights;
    self->abstract.numLights = numLights;

    self->abstract.volumeData = 0;

    self->abstract.materials = ( const uniform ExtendedOBJMaterial* uniform* uniform )materials;
    self->abstract.numMaterials = numMaterials;
}
