/* Copyright (c) 2015-2017, EPFL/Blue Brain Project
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

struct BasicRenderer
{
    AbstractRenderer abstract;
};

/**
    Renderer a pixel color according to a given location in the screen space.
    @param self Pointer to current renderer
    @param sample Screen sample containing information about the ray, and the
           location in the screen space.
*/
inline vec3f BasicRenderer_shadeRay(const uniform BasicRenderer* uniform self,
                                    varying ScreenSample& sample)
{
    Ray ray = sample.ray;
    varying vec3f color = make_vec3f(0.f);
    sample.z = 1.f;

    traceRay(self->abstract.super.model, ray);

    if (ray.geomID < 0)
    {
        // No intersection. Return skybox color
        color = make_vec3f(skyboxMapping((Renderer*)self, ray,
                                         self->abstract.numMaterials,
                                         self->abstract.materials));
        sample.alpha = 0.f;
        return color;
    }

    DifferentialGeometry dg;
    postIntersect(self->abstract.super.model, dg, ray,
                  DG_NS | DG_NG | DG_NORMALIZE | DG_FACEFORWARD |
                      DG_MATERIALID | DG_COLOR);

    uniform Material* material = dg.material;
    uniform ExtendedOBJMaterial* objMaterial =
        (uniform ExtendedOBJMaterial*)material;

    vec3f Kd = make_vec3f(0.f);
    if (!objMaterial)
        Kd = make_vec3f(dg.color);
    else
        foreach_unique(mat in objMaterial) Kd = mat->Kd * make_vec3f(dg.color);

    // Head-light shading
    const vec3f intersection = dg.P + self->abstract.super.epsilon * dg.Ns;
    const vec3f headLight = normalize(ray.org - intersection);
    color = Kd * max(0.f, dot(headLight, dg.Ns));
    sample.alpha = 1.f;

    // Z-Depth
    sample.z = log(ray.t) / 10.f;

    return color;
}

void BasicRenderer_renderSample(uniform Renderer* uniform _self,
                                void* uniform perFrameData,
                                varying ScreenSample& sample)
{
    uniform BasicRenderer* uniform self =
        (uniform BasicRenderer * uniform)_self;
    sample.ray.time = self->abstract.timestamp;
    sample.rgb = BasicRenderer_shadeRay(self, sample);
}

// Exports (called from C++)
export void* uniform BasicRenderer_create(void* uniform cppE)
{
    uniform BasicRenderer* uniform self = uniform new uniform BasicRenderer;
    Renderer_Constructor(&self->abstract.super, cppE);
    self->abstract.super.renderSample = BasicRenderer_renderSample;
    return self;
}

export void BasicRenderer_set(void* uniform _self, const uniform vec3f& bgColor,
                              const uniform float& timestamp,
                              const uniform int& spp, void** uniform lights,
                              uniform int32 numLights, void** uniform materials,
                              uniform int32 numMaterials)
{
    uniform BasicRenderer* uniform self =
        (uniform BasicRenderer * uniform)_self;

    self->abstract.bgColor = bgColor;
    self->abstract.timestamp = timestamp;
    self->abstract.spp = spp;

    self->abstract.lights = (const uniform Light* uniform* uniform)lights;
    self->abstract.numLights = numLights;

    self->abstract.materials =
        (const uniform ExtendedOBJMaterial* uniform* uniform)materials;
    self->abstract.numMaterials = numMaterials;
}
