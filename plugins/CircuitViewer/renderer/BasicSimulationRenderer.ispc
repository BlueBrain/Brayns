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

#include "SimulationRenderer.ih"

inline vec3f BasicSimulationRenderer_shadeRay(
    const uniform SimulationRenderer* uniform self,
    varying ScreenSample& sample)
{
    Ray ray = sample.ray;
    ray.time = inf;
    sample.z = inf;
    sample.alpha = 0;

    traceRay(self->super.super.model, ray);

    if (ray.geomID < 0)
    {
        // No intersection
        vec4f skybox =
            skyboxMapping((Renderer*)self, ray, self->super.bgMaterial);
        sample.alpha = skybox.w;
        return make_vec3f(skybox);
    }

    // Intersection found
    // Retreive information about the geometry, typically geometry ID,
    // normal to the surface, material ID, texture coordinates, etc.
    DifferentialGeometry dg;
    postIntersect(self->super.super.model, dg, ray,
                  DG_NG | DG_NS | DG_NORMALIZE | DG_FACEFORWARD |
                      DG_MATERIALID | DG_COLOR | DG_TEXCOORD);

    // Material attributes
    const uniform DefaultMaterial* material =
        (const uniform DefaultMaterial*)dg.material;

    vec3f Kd;
    if (!material)
    {
        Kd = make_vec3f(dg.color);
    }
    else
    {
        foreach_unique(mat in material) { Kd = make_vec3f(dg.color) * mat->Kd; }
    }

    vec3f colorContribution = Kd;

    if (hasSimulationMapping(dg))
    {
        // Get simulation value from geometry
        const vec4f color = getSimulationColor(*self, dg);
        colorContribution = make_vec3f(color);
    }

    // Head-light shading
    const float cosNL = max(0.f, dot(neg(ray.dir), dg.Ns));
    colorContribution = colorContribution * cosNL;
    sample.alpha = 1;
    sample.z = ray.t;
    return make_vec3f(colorContribution);
}

void BasicSimulationRenderer_renderSample(uniform Renderer* uniform _self,
                                          void* uniform perFrameData,
                                          varying ScreenSample& sample)
{
    uniform SimulationRenderer* uniform self =
        (uniform SimulationRenderer * uniform)_self;
    sample.rgb = BasicSimulationRenderer_shadeRay(self, sample);
}

// Exports (called from C++)
export void* uniform BasicSimulationRenderer_create(void* uniform cppE)
{
    uniform SimulationRenderer* uniform self =
        uniform new uniform SimulationRenderer;
    Renderer_Constructor(&self->super.super, cppE);
    self->super.super.renderSample = BasicSimulationRenderer_renderSample;
    return self;
}

export void BasicSimulationRenderer_set(void* uniform _self,
                                        void* uniform bgMaterial,
                                        const uniform int& spp,
                                        uniform float* uniform simulationData,
                                        const uniform int64 simulationDataSize)
{
    uniform SimulationRenderer* uniform self =
        (uniform SimulationRenderer * uniform)_self;

    self->super.bgMaterial = (uniform DefaultMaterial * uniform)bgMaterial;
    self->super.super.spp = spp;

    self->simulationData = (uniform float* uniform)simulationData;
    self->simulationDataSize = simulationDataSize;
}
