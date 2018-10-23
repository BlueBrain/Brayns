/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
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

#include "utils/AbstractRenderer.ih"

uniform const float nearFarThreshold = 0.2f;

struct ProximityRenderer
{
    AbstractRenderer super;

    int randomNumber;

    bool surfaceShadingEnabled;
    bool electronShadingEnabled;

    vec3f nearColor;
    vec3f farColor;
    float detectionDistance;
    bool detectionOnDifferentMaterial;

    float alphaCorrection;
};

inline vec3f ProximityRenderer_shadeRay(
    const uniform ProximityRenderer* uniform self, varying ScreenSample& sample)
{
    Ray ray = sample.ray;
    vec4f color = make_vec4f(0.f);
    sample.alpha = 0.f;

    int iteration = 0;
    while (color.w < 1.f && iteration < NB_MAX_REBOUNDS)
    {
        traceRay(self->super.super.model, ray);

        if (ray.geomID < 0)
        {
            const vec4f bgColor =
                skyboxMapping((Renderer*)self, ray, self->super.bgMaterial);
            composite(bgColor, color, self->alphaCorrection);
            return make_vec3f(color);
        }

        DifferentialGeometry dg;
        postIntersect(self->super.super.model, dg, ray,
                      DG_NG | DG_NS | DG_NORMALIZE | DG_FACEFORWARD |
                          DG_MATERIALID | DG_COLOR | DG_TEXCOORD);

        uniform Material* material = dg.material;
        uniform DefaultMaterial* objMaterial =
           (uniform DefaultMaterial*)material;

        vec3f normal = dg.Ns;
        const vec3f P = dg.P + dg.epsilon * dg.Ng;

        bool continueWithSurfaceShading = true;
        varying vec3f ao_dir =
            getRandomVector(self->super.super.fb->size.x, sample, normal,
                            self->randomNumber);

        if (dot(ao_dir, normal) < 0.f)
            ao_dir = ao_dir * -1.f;
        Ray ao_ray = ray;
        ao_ray.org = P;
        ao_ray.dir = ao_dir;
        ao_ray.t0 = dg.epsilon;
        ao_ray.t = self->detectionDistance;
        ao_ray.geomID = -1;

        traceRay(self->super.super.model, ao_ray);
        if (ao_ray.geomID >= 0)
        {
            continueWithSurfaceShading = false;

            postIntersect(self->super.super.model, dg, ao_ray, DG_MATERIALID);

            const bool doDetectionTest = self->detectionOnDifferentMaterial
                                             ? material != dg.material
                                             : true;
            if (doDetectionTest)
            {
                const float a = ao_ray.t / self->detectionDistance;
                const vec4f touchColor =
                    make_vec4f(a > nearFarThreshold ? self->nearColor
                                                    : self->farColor,
                               1.f);
                composite(touchColor, color, self->alphaCorrection);
                sample.alpha = 1.f;
                if (iteration == 0)
                    sample.z = ray.t;
            }
        }

        if (continueWithSurfaceShading && self->surfaceShadingEnabled)
        {
            sample.alpha = 1.f;
            if (iteration == 0)
                sample.z = ray.t;

            vec3f Kd;
            if (!objMaterial)
                Kd = make_vec3f(dg.color);
            else
                foreach_unique(mat in objMaterial) Kd =
                    mat->Kd * make_vec3f(dg.color);

            for (uniform int i = 0;
                 self->super.lights && i < self->super.numLights; i++)
            {
                const uniform Light* uniform light = self->super.lights[i];

                const vec2f samplingLocation = make_vec2f(0.f);
                const Light_SampleRes lightSample =
                    light->sample(light, dg, samplingLocation);
                const float radiance = reduce_max(lightSample.weight);

                if (reduce_max(radiance) > 0.f)
                {
                    float cosNL = 0.f;
                    if (self->electronShadingEnabled)
                    {
                        const vec3f me = normalize(ray.org - P);
                        const float angle = max(0.f, dot(me, normal));
                        cosNL = 1.f - angle;
                    }
                    else
                        cosNL = max(0.f, dot(lightSample.dir, normal));

                    const vec4f shadedColor =
                        make_vec4f(Kd * cosNL * radiance, 1.f);

                    composite(shadedColor, color, self->alphaCorrection);
                }
            }
            break;
        }

        setRay(ray, dg.P, ray.dir);
        ray.t0 = dg.epsilon;
        ray.t = inf;
        ray.primID = -1;
        ray.geomID = -1;
        ray.instID = -1;
        ++iteration;
    }

    // Depth and Alpha

    return make_vec3f(color);
}

void ProximityRenderer_renderSample(uniform Renderer* uniform _self,
                                    void* uniform perFrameData,
                                    varying ScreenSample& sample)
{
    uniform ProximityRenderer* uniform self =
        (uniform ProximityRenderer * uniform)_self;
    sample.ray.time = self->super.timestamp;
    sample.rgb = ProximityRenderer_shadeRay(self, sample);
}

// Exports (called from C++)
export void* uniform ProximityRenderer_create(void* uniform cppE)
{
    uniform ProximityRenderer* uniform self =
        uniform new uniform ProximityRenderer;

    Renderer_Constructor(&self->super.super, cppE);
    self->super.super.renderSample = ProximityRenderer_renderSample;
    return self;
}

export void ProximityRenderer_set(
    void* uniform _self, void* uniform bgMaterial,
    const uniform vec3f& nearColor, const uniform vec3f& farColor,
    const uniform float& detectionDistance,
    const uniform bool& detectionOnDifferentMaterial,
    const uniform int& randomNumber, const uniform float& timestamp,
    const uniform int& spp, const uniform bool& surfaceShadingEnabled,
    const uniform bool& electronShadingEnabled, void** uniform lights,
    uniform int32 numLights, const uniform float& alphaCorrection)
{
    uniform ProximityRenderer* uniform self =
        (uniform ProximityRenderer * uniform)_self;

    self->super.super.spp = spp;
    self->super.bgMaterial = (uniform DefaultMaterial * uniform)bgMaterial;
    self->super.lights = (const uniform Light* uniform* uniform)lights;
    self->super.numLights = numLights;
    self->super.timestamp = timestamp;

    self->randomNumber = randomNumber;
    self->surfaceShadingEnabled = surfaceShadingEnabled;
    self->electronShadingEnabled = electronShadingEnabled;
    self->nearColor = nearColor;
    self->farColor = farColor;
    self->detectionDistance = detectionDistance;
    self->detectionOnDifferentMaterial = detectionOnDifferentMaterial;

    self->alphaCorrection = alphaCorrection;
}
