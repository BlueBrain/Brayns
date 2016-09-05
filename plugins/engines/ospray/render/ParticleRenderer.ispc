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
#include <plugins/engines/ospray/render/utils/AbstractRenderer.ih>


const float NEST_TIMESTEP = 0.1f;

struct ParticleRenderer
{
    AbstractRenderer abstract;

    uniform float* uniform simulationData;
    uniform vec4f* uniform colorMap;
    uniform float* uniform colorMapEmissionData;
    uint32 colorMapSize;
    float colorMapMinValue;
    float colorMapRange;
    float threshold;
};

inline varying vec4f getSimulationValue(
    const uniform ParticleRenderer* uniform self,
    DifferentialGeometry& dg,
    const int32 index )
{
    varying vec4f color = make_vec4f( 1.f, 0.f, 0.f, 0.5f );
    if( !self->simulationData || !self->transferFunction )
        return color;

    const varying float timestamp = self->simulationData[ index ];
    const varying float spikeValue = ( timestamp < 0.f ) ? 0.f :
        min(1.f, max( 0.f, 1.f - NEST_TIMESTEP * ( self->abstract.timestamp - timestamp )));

    if( dg.st.x > self->transferFunctionSize )
        return color;

    color = self->transferFunction[ dg.st.x ];
    color.w = spikeValue;
    return color;
}

inline vec3f ParticleRenderer_shadeRay(
    const uniform ParticleRenderer* uniform self,
    varying ScreenSample& sample)
{
    Ray ray = sample.ray;

    varying vec3f intersectionColors = make_vec3f( 0.f );
    sample.z = 1.f;
    varying int depth = 0;
    varying float path_opacity = 0.f;
    varying float opacity = 0.f;

    varying bool moreRebounds = true;
    while( moreRebounds /*&& depth < NB_MAX_REBOUNDS*/ )
    {
        traceRay( self->abstract.super.model, ray );

        if( ray.geomID < 0 )
        {
            // No intersection. Return skybox color
            intersectionColors = intersectionColors * 0.5f + 0.5f *
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

            // Get simulation value from geometry ID
            varying float lightEmission;
            const vec4f simulationValue = getSimulationValue( self, dg, lightEmission );
            intersectionColors = intersectionColors * 0.5f + 0.5f * make_vec3f( simulationValue );

#if 0
            // Process material attributes
            const uniform Material* material = dg.material;
            const uniform ExtendedOBJMaterial* objMaterial =
                    ( const uniform ExtendedOBJMaterial* ) material;
            if( !objMaterial )
                // Diffuse color
                intersectionColors = intersectionColors * 0.5f + 0.5f * make_vec3f( dg.color );
            else
                foreach_unique( mat in objMaterial )
                    // Diffuse color
                    intersectionColors =
                        intersectionColors * 0.5f + 0.5f * mat->Kd;
#endif // 0

            ray.t0 = ray.t + self->abstract.super.epsilon;
            ray.t = infinity;
            ray.primID = -1;
            ray.geomID = -1;
            ray.instID = -1;
        }
        ++depth;
    }

    return intersectionColors;
}

void ParticleRenderer_renderSample(
    uniform Renderer* uniform _self,
    void * uniform perFrameData,
    varying ScreenSample& sample)
{
    uniform ParticleRenderer* uniform self =
            ( uniform ParticleRenderer* uniform )_self;
    sample.ray.time = self->abstract.timestamp;
    sample.rgb = ParticleRenderer_shadeRay( self, sample );
}

// Exports (called from C++)
export void* uniform ParticleRenderer_create( void* uniform cppE )
{
    uniform ParticleRenderer* uniform self =
            uniform new uniform ParticleRenderer;
    Renderer_Constructor( &self->abstract.super, cppE );
    self->abstract.super.renderSample = ParticleRenderer_renderSample;
    return self;
}

export void ParticleRenderer_set(
        void* uniform _self,
        const uniform vec3f& bgColor,
        const uniform int& randomNumber,
        const uniform float& timestamp,
        const uniform int& spp,
        void** uniform materials,
        const uniform int32 numMaterials,
        uniform float* uniform simulationData,
        uniform vec4f* uniform colormap,
        uniform float* uniform colormapEmissionData,
        const uniform int32 colorMapSize,
        const uniform float colorMapMinValue,
        const uniform float colorMapRange,
        const uniform float& threshold )
{
    uniform ParticleRenderer* uniform self = ( uniform ParticleRenderer* uniform )_self;

    self->abstract.bgColor = bgColor;
    self->abstract.randomNumber = randomNumber;
    self->abstract.timestamp = timestamp;
    self->abstract.spp = spp;
    self->abstract.electronShadingEnabled = electronShadingEnabled;

    self->abstract.lights = ( const uniform Light* uniform* uniform )lights;
    self->abstract.numLights = numLights;

    self->abstract.materials = ( const uniform ExtendedOBJMaterial* uniform* uniform )materials;
    self->abstract.numMaterials = numMaterials;

    self->simulationData = (uniform float* uniform)simulationData;
    self->transferFunction = (uniform vec4f* uniform)transferFunction;
    self->transferFunctionEmissionData = (uniform float* uniform)transferFunctionEmissionData;
    self->transferFunctionSize = transferFunctionSize;
}
