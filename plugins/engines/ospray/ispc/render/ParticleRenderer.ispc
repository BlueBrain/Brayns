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

const float NEST_TIMESTEP = 0.1f;

struct ParticleRenderer
{
    AbstractRenderer abstract;

    uniform float* uniform simulationData;
    int64 simulationDataSize;
    uniform vec4f* uniform transferFunction;
    uniform float* uniform transferFunctionEmissionData;
    uint32 transferFunctionSize;
};

inline varying vec4f
    getSimulationValue(const uniform ParticleRenderer* uniform self,
                       DifferentialGeometry& dg, const int32 index)
{
    varying vec4f color = make_vec4f(1.f, 0.f, 0.f, 0.5f);
    if (!self->simulationData || !self->transferFunction)
        return color;

    varying int32 idx = max(0, min(index, self->simulationDataSize));
    const varying float timestamp = self->simulationData[idx];
    const varying float spikeValue =
        (timestamp < 0.f)
            ? 0.f
            : min(1.f,
                  max(0.f, 1.f - (NEST_TIMESTEP * self->abstract.timestamp -
                                  timestamp)));

    if (dg.st.x > self->transferFunctionSize)
        return color;

    color = self->transferFunction[dg.st.x];
    color.w = spikeValue;
    return color;
}

inline vec3f ParticleRenderer_shadeRay(
    const uniform ParticleRenderer* uniform self, varying ScreenSample& sample)
{
    Ray ray = sample.ray;

    varying float zDepth = 0.f;
    sample.z = 1.f;
    sample.alpha = 0.f;
    varying int depth = 0;
    varying float path_opacity = 0.f;
    varying float opacity = 0.f;

    varying vec3f intersectionColor = make_vec3f(
        skyboxMapping((Renderer*)self, ray, self->abstract.bgMaterial));

    while (path_opacity < 1.f && depth < NB_MAX_REBOUNDS)
    {
        traceRay(self->abstract.super.model, ray);

        if (ray.geomID < 0)
        {
            sample.z = 1.f - log(zDepth) / 10.f;
            break;
        }

        // Intersection found
        if (depth == 0)
            sample.alpha = 1.f;

        // Retrieve information about the geometry, typically geometry ID,
        // normal to the surface, material ID, texture coordinates, etc.
        DifferentialGeometry dg;
        postIntersect(self->abstract.super.model, dg, ray,
                      DG_NG | DG_NS | DG_NORMALIZE | DG_FACEFORWARD |
                          DG_MATERIALID | DG_COLOR | DG_TEXCOORD);

        if (ray.geomID == 1)
        {
            // Non simulated geometry
            const uniform Material* material = dg.material;
            const uniform ExtendedOBJMaterial* objMaterial =
                (const uniform ExtendedOBJMaterial*)material;
            varying vec3f Kd;
            if (!objMaterial)
            {
                Kd = make_vec3f(dg.color);
                opacity = dg.color.w;
            }
            else
                foreach_unique(mat in objMaterial)
                {
                    Kd = mat->Kd;
                    opacity = mat->d;
                }

            const varying vec3f intersection = dg.P + dg.epsilon * dg.Ng;
            const varying vec3f viewer = normalize(ray.org - intersection);
            const varying float el = max(0.f, dot(viewer, dg.Ng));
            varying float cosNL = 1.f - el;
            intersectionColor =
                intersectionColor * (1.f - opacity) + opacity * Kd * cosNL;
        }
        else
        {
            if (depth == 0)
                sample.z = ray.primID;

            // Get simulation value from geometry
            const vec4f simulationValue =
                getSimulationValue(self, dg, ray.primID);

            // Spike value is different from zero, meaning that the neuron
            // is activated and
            // neuron color should be considered. If the spike value is
            // zero, the
            // intersection is ignored
            opacity = simulationValue.w;
            if (opacity > 0.f)
            {
                intersectionColor = intersectionColor +
                                    make_vec3f(simulationValue) * opacity *
                                        (1.f - path_opacity);
                path_opacity = path_opacity + opacity * (1.f - path_opacity);
            }
        }

        // Z-Depth
        zDepth += opacity * ray.t;
        sample.z = log(zDepth) / 10.f;

        // Next ray
        ray.t0 = ray.t + dg.epsilon * 5.f;
        ray.t = infinity;
        ray.primID = -1;
        ray.geomID = -1;
        ray.instID = -1;

        ++depth;
    }

    // Z-Depth
    if (depth >= NB_MAX_REBOUNDS)
        sample.z = 1.f - log(zDepth) / 10.f;

    return intersectionColor;
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
    const uniform int& randomNumber, const uniform float& timestamp,
    const uniform int& spp, uniform float* uniform simulationData,
    const uniform int64 simulationDataSize,
    uniform vec4f* uniform transferFunction,
    uniform float* uniform transferFunctionEmissionData,
    const uniform uint32 transferFunctionSize)
{
    uniform ParticleRenderer* uniform self =
        (uniform ParticleRenderer * uniform)_self;

    self->abstract.bgMaterial =
        (uniform ExtendedOBJMaterial * uniform)bgMaterial;

    self->abstract.randomNumber = randomNumber;
    self->abstract.timestamp = timestamp;
    self->abstract.spp = spp;

    self->simulationData = (uniform float* uniform)simulationData;
    self->simulationDataSize = simulationDataSize;
    self->transferFunction = (uniform vec4f * uniform)transferFunction;
    self->transferFunctionEmissionData =
        (uniform float* uniform)transferFunctionEmissionData;
    self->transferFunctionSize = transferFunctionSize;
}
