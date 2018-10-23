/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
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

#include "utils/SimulationRenderer.ih"

struct BasicSimulationRenderer
{
    SimulationRenderer super;

    // Shading attributes
    float alphaCorrection;
};

inline vec3f BasicSimulationRenderer_shadeRay(
    const uniform BasicSimulationRenderer* uniform self,
    varying ScreenSample& sample)
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
        traceRay(self->super.super.super.model, ray);

        if (ray.geomID < 0)
        {
            // No intersection
            colorContribution = skyboxMapping((Renderer*)self, ray,
                                              self->super.super.bgMaterial);
            colorContribution.w = 1.f;
            composite(colorContribution, intersectionColor,
                      self->alphaCorrection);
            break;
        }

        // Intersection found
        // Retreive information about the geometry, typically geometry ID,
        // normal to the surface, material ID, texture coordinates, etc.
        DifferentialGeometry dg;
        postIntersect(self->super.super.super.model, dg, ray,
                      DG_NG | DG_NS | DG_NORMALIZE | DG_FACEFORWARD |
                          DG_MATERIALID | DG_COLOR | DG_TEXCOORD);

        // Material attributes
        const uniform DefaultMaterial* material =
            (const uniform DefaultMaterial*)dg.material;
        vec3f Kd;
        if (!material)
        {
            Kd = make_vec3f(dg.color);
            opacity = dg.color.w;
        }
        else
        {
            foreach_unique(mat in material)
            {
                Kd = make_vec3f(dg.color) * mat->Kd;
                opacity = dg.color.w * mat->d;
            }
        }

        if (depth == 0)
        {
            pathOpacity = opacity;
            sample.z = ray.t;
        }
        pathOpacity *= 1.f + opacity;
        colorContribution = make_vec4f(Kd, pathOpacity);

        const uniform SimulationMaterial* simMaterial =
            (const uniform SimulationMaterial*)dg.material;

        if (simMaterial && simMaterial->getSimulationOffset)
        {
            // Get simulation value from geometry
            const vec4f color = getSimulationColor(self->super, dg);
            colorContribution =
                make_vec4f(make_vec3f(colorContribution) * (1.f - color.w) +
                               make_vec3f(color) * color.w,
                           colorContribution.w);
        }

        // Head-light shading
        const float cosNL = max(0.f, dot(neg(ray.dir), dg.Ns));
        colorContribution = colorContribution * cosNL;

        // Compositing according to alpha correction
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

void BasicSimulationRenderer_renderSample(uniform Renderer* uniform _self,
                                          void* uniform perFrameData,
                                          varying ScreenSample& sample)
{
    uniform BasicSimulationRenderer* uniform self =
        (uniform BasicSimulationRenderer * uniform)_self;
    sample.rgb = BasicSimulationRenderer_shadeRay(self, sample);
}

// Exports (called from C++)
export void* uniform BasicSimulationRenderer_create(void* uniform cppE)
{
    uniform BasicSimulationRenderer* uniform self =
        uniform new uniform BasicSimulationRenderer;
    Renderer_Constructor(&self->super.super.super, cppE);
    self->super.super.super.renderSample = BasicSimulationRenderer_renderSample;
    return self;
}

export void BasicSimulationRenderer_set(void* uniform _self,
                                        void* uniform bgMaterial,
                                        const uniform int& spp,
                                        uniform float* uniform simulationData,
                                        const uniform int64 simulationDataSize,
                                        const uniform float& alphaCorrection)
{
    uniform BasicSimulationRenderer* uniform self =
        (uniform BasicSimulationRenderer * uniform)_self;

    self->super.super.bgMaterial =
        (uniform DefaultMaterial * uniform)bgMaterial;

    self->super.super.super.spp = spp;

    self->super.simulationData = (uniform float* uniform)simulationData;
    self->super.simulationDataSize = simulationDataSize;

    self->alphaCorrection = alphaCorrection;
}
