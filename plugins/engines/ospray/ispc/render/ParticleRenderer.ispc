/* Copyright (c) 2015-2017, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
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

#include <plugins/engines/ospray/ispc/render/utils/AbstractRenderer.ih>

struct ParticleRenderer
{
    AbstractRenderer abstract;

    // Transfer function / Color map attributes
    uniform vec4f* uniform colorMap;
    uniform vec3f* uniform emissionIntensitiesMap;
    uint32 colorMapSize;
    float colorMapMinValue;
    float colorMapRange;

    // Simulation data
    uniform float* uniform simulationData;
    int64 simulationDataSize;

    // Alpha correction
    float alphaCorrection;
};

inline vec4f getSimulationValue(const uniform ParticleRenderer* uniform self,
                                DifferentialGeometry& dg)
{
    vec4f color = make_vec4f(1.f, 0.f, 0.f, 0.5f);
    if (!self->simulationData || !self->colorMap)
        return color;

    float value = 0.f;
    const uint64 index = (uint64)(dg.st.x * OFFSET_MAGIC) << 32 |
                         (uint32)(dg.st.y * OFFSET_MAGIC);

    if (index < self->simulationDataSize)
        value = self->simulationData[index];
    else
        // Value offset is out of range, return error color
        return color;

    // Normalize the value according colormap size
    const float normalizedValue =
        (value - self->colorMapMinValue) / self->colorMapRange;

    const uint32 lookupIndex =
        max(0.f, min(normalizedValue, 1.f)) * (self->colorMapSize - 1);
    color = self->colorMap[lookupIndex];

    if (self->emissionIntensitiesMap)
        color = make_vec4f(make_vec3f(color) +
                               self->emissionIntensitiesMap[lookupIndex],
                           color.w);
    return color;
}

inline vec3f ParticleRenderer_shadeRay(
    const uniform ParticleRenderer* uniform self, varying ScreenSample& sample)
{
    Ray ray = sample.ray;
    ray.time = inf;
    sample.z = inf;

    float zDepth = 0.f;
    int depth = 0;
    float opacity = 0.f;
    float pathOpacity = 0.f;

    vec4f intersectionColor = make_vec4f(0.f);
    vec4f colorContribution = make_vec4f(0.f);

    while (pathOpacity < 1.f)
    {
        traceRay(self->abstract.super.model, ray);

        if (ray.geomID < 0)
        {
            // No intersection
            colorContribution =
                skyboxMapping((Renderer*)self, ray, self->abstract.bgMaterial);
            colorContribution.w = 1.f;
            composite(colorContribution, intersectionColor,
                      self->alphaCorrection);
            break;
        }

        // Intersection found
        // Retreive information about the geometry, typically geometry ID,
        // normal to the surface, material ID, texture coordinates, etc.
        DifferentialGeometry dg;
        postIntersect(self->abstract.super.model, dg, ray,
                      DG_NG | DG_NS | DG_NORMALIZE | DG_FACEFORWARD |
                          DG_MATERIALID | DG_COLOR | DG_TEXCOORD);

        const uniform Material* material = dg.material;
        const uniform ExtendedOBJMaterial* objMaterial =
            (const uniform ExtendedOBJMaterial*)material;
        vec3f Kd;
        if (!objMaterial)
        {
            Kd = make_vec3f(dg.color);
            opacity = dg.color.w;
        }
        else
            foreach_unique(mat in objMaterial)
            {
                Kd = make_vec3f(dg.color) * mat->Kd;
                opacity = dg.color.w * mat->d;
            }

        if (depth == 0)
        {
            pathOpacity = opacity;
            sample.z = ray.t;
        }
        pathOpacity *= 1.f + opacity;

        // Head-light shading
        const float cosNL = max(0.f, dot(neg(ray.dir), dg.Ns));
        colorContribution = make_vec4f(Kd * cosNL, pathOpacity);

        if (objMaterial->castSimulationData == 1)
        {
            // Get simulation value from geometry
            const vec4f simulationColor = getSimulationValue(self, dg);
            colorContribution =
                make_vec4f(make_vec3f(colorContribution) *
                                   (1.f - simulationColor.w) +
                               make_vec3f(simulationColor) * simulationColor.w,
                           colorContribution.w);
        }
        composite(colorContribution, intersectionColor, self->alphaCorrection);

        // Next ray
        ray.t0 = ray.t + dg.epsilon;
        ray.t = infinity;
        ray.primID = -1;
        ray.geomID = -1;
        ray.instID = -1;
        ++depth;
    }

    // Alpha
    sample.alpha = pathOpacity;

    return make_vec3f(intersectionColor);
}

void ParticleRenderer_renderSample(uniform Renderer* uniform _self,
                                   void* uniform perFrameData,
                                   varying ScreenSample& sample)
{
    uniform ParticleRenderer* uniform self =
        (uniform ParticleRenderer * uniform)_self;
    sample.ray.time = self->abstract.timestamp;
    sample.rgb = ParticleRenderer_shadeRay(self, sample);
}

// Exports (called from C++)
export void* uniform ParticleRenderer_create(void* uniform cppE)
{
    uniform ParticleRenderer* uniform self =
        uniform new uniform ParticleRenderer;
    Renderer_Constructor(&self->abstract.super, cppE);
    self->abstract.super.renderSample = ParticleRenderer_renderSample;
    return self;
}

export void ParticleRenderer_set(
    void* uniform _self, void* uniform bgMaterial,
    const uniform float& timestamp, const uniform int& spp,
    uniform float* uniform simulationData,
    const uniform int64 simulationDataSize, uniform vec4f* uniform colormap,
    uniform vec3f* uniform emissionIntensitiesMap,
    const uniform int32 colorMapSize, const uniform float& colorMapMinValue,
    const uniform float& colorMapRange, const uniform float& alphaCorrection)
{
    uniform ParticleRenderer* uniform self =
        (uniform ParticleRenderer * uniform)_self;

    self->abstract.bgMaterial =
        (uniform ExtendedOBJMaterial * uniform)bgMaterial;

    self->abstract.timestamp = timestamp;
    self->abstract.super.spp = spp;

    self->colorMap = (uniform vec4f * uniform)colormap;
    self->emissionIntensitiesMap =
        (uniform vec3f * uniform)emissionIntensitiesMap;
    self->colorMapSize = colorMapSize;
    self->colorMapMinValue = colorMapMinValue;
    self->colorMapRange = colorMapRange;

    self->simulationData = (uniform float* uniform)simulationData;
    self->simulationDataSize = simulationDataSize;

    self->alphaCorrection = alphaCorrection;
}
