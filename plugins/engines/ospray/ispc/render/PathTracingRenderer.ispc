/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Authors: Samuel Lapere <samuel.lapere@epfl.ch>
 *                      Cyrille Favreau <cyrille.favreau@epfl.ch>
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
#include <ospray/SDK/common/Ray.ih>
#include <plugins/engines/ospray/ispc/render/utils/AbstractRenderer.ih>

struct PathTracingRenderer
{
    AbstractRenderer abstract;
};

inline vec3f mix(const vec3f a, const vec3f b, float t)
{
    const float ax = a.x, ay = a.y, az = a.z;
    return make_vec3f(ax + t * (b.x - ax), ay + t * (b.y - ay),
                      az + t * (b.z - az));
}

/**
    Renderer a pixel color according to a given location in the screen space.
    @param self Pointer to current renderer
    @param sample Screen sample containing information about the ray, and the
           location in the screen space.
*/
inline vec3f PathTracingRenderer_shadeRay(
    const uniform PathTracingRenderer* uniform self,
    varying ScreenSample& sample)
{
    Ray ray = sample.ray;
    varying vec3f color = make_vec3f(0.f);

    sample.z = inf;
    sample.alpha = 0.f;

    traceRay(self->abstract.super.model, ray);

    if (ray.geomID < 0)
    {
        // No intersection. Return skybox color
        color = make_vec3f(skyboxMapping((Renderer*)self, ray,
                                         self->abstract.bgMaterial)) *
                DEFAULT_SKY_POWER_ZERO_BOUNCE;

        sample.alpha = 0.f;
        return color;
    }

    // else, we've got a hit!
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

    Ray localray = ray;
    vec3f accucolor = make_vec3f(0.f);
    vec3f mask = make_vec3f(1.f);

    varying RandomTEA* uniform rng =
        createRandomNumberGenerator(&self->abstract.super, sample);

    // path tracing loop
    for (int bounces = 0; bounces < NB_MAX_PATH_TRACING_REBOUNDS; bounces++)
    {
        traceRay(self->abstract.super.model, localray);

        // if ray misses scene (no hit occurs), return background colour
        if (localray.geomID < 0)
        {
            const vec3f bgcol =
                make_vec3f(skyboxMapping((Renderer*)self, localray,
                                         self->abstract.bgMaterial)) *
                DEFAULT_SKY_POWER;

            accucolor = accucolor + mask * bgcol;
            break;
        }
        DifferentialGeometry dg;
        postIntersect(self->abstract.super.model, dg, localray,
                      DG_NG | DG_NS | DG_NORMALIZE | DG_FACEFORWARD |
                          DG_MATERIALID | DG_COLOR);

        uniform Material* material = dg.material;
        uniform ExtendedOBJMaterial* objMaterial =
            (uniform ExtendedOBJMaterial*)material;
        if (!objMaterial)
            Kd = make_vec3f(dg.color);
        else
            foreach_unique(mat in objMaterial) Kd =
                mat->Kd * make_vec3f(dg.color);

        // compute ray direction of uniformly random diffuse ray
        const vec3f raydir =
            getRandomVector(sample, dg.Ns, self->abstract.super.epsilon);

        // origin of new ray in path is hitpoint of previous ray in path
        const vec3f hitpoint = dg.P + self->abstract.super.epsilon * dg.Ns;
        setRay(localray, hitpoint, raydir);
        localray.t0 = self->abstract.super.epsilon;
        localray.t = self->abstract.ambientOcclusionDistance;

        if (self->abstract.shadows > 0.f)
        {
            // Direct lighting
            for (uniform int i = 0;
                 self->abstract.lights && i < self->abstract.numLights; ++i)
            {
                const uniform Light* uniform light = self->abstract.lights[i];
                const vec2f s = make_vec2f(0.5f);
                const varying Light_SampleRes lightSample =
                    light->sample(light, dg, s);
                const vec3f radiance = lightSample.weight;
                if (reduce_max(radiance) > 0.f)
                {
                    const vec3f lightDirection = lightSample.dir;
                    const vec3f sunSampleDirection =
                        getConeSample(lightDirection, rng,
                                      self->abstract.softShadows);
                    const float sunLight =
                        dot(dg.Ns,
                            sunSampleDirection); // cosine weighted contribution

                    if (sunLight > 0.f) // if surface is facing light
                    {
                        // create and trace a shadowray towards the sun
                        Ray sunRay;
                        setRay(sunRay, hitpoint, sunSampleDirection);
                        if (isOccluded(self->abstract.super.model, sunRay))
                            // take surface color at this bounce's hitpoint into
                            // account
                            Kd = Kd * (1.f - self->abstract.shadows);
                    }
                    else
                        Kd = Kd * (1.f - self->abstract.shadows);
                }
            }
        }
        mask = mask * Kd;
    } // end bounces

    // CHECK THIS!!! WITH OR WITHOUT * MASK
    // results are very different. Color bleeding is much stronger with mask
    color = accucolor * mask;

    // Z-Depth
    sample.z = ray.t;
    sample.alpha = 1.f;

    return color;
}

void PathTracingRenderer_renderSample(uniform Renderer* uniform _self,
                                      void* uniform perFrameData,
                                      varying ScreenSample& sample)
{
    uniform PathTracingRenderer* uniform self =
        (uniform PathTracingRenderer * uniform) _self;
    sample.ray.time = self->abstract.timestamp;
    sample.rgb = PathTracingRenderer_shadeRay(self, sample);
}

// Exports (called from C++)
export void* uniform PathTracingRenderer_create(void* uniform cppE)
{
    uniform PathTracingRenderer* uniform self =
        uniform new uniform PathTracingRenderer;
    Renderer_Constructor(&self->abstract.super, cppE);
    self->abstract.super.renderSample = PathTracingRenderer_renderSample;
    return self;
}

export void PathTracingRenderer_set(
    void* uniform _self, void* uniform bgMaterial,
    const uniform float& timestamp, const uniform int& spp,
    void** uniform lights, uniform int32 numLights,
    const uniform float& shadows, const uniform float& softShadows,
    const uniform float& ambientOcclusionStrength,
    const uniform float& ambientOcclusionDistance)
{
    uniform PathTracingRenderer* uniform self =
        (uniform PathTracingRenderer * uniform) _self;

    self->abstract.bgMaterial =
        (uniform ExtendedOBJMaterial * uniform) bgMaterial;
    self->abstract.timestamp = timestamp;
    self->abstract.spp = spp;

    self->abstract.lights = (const uniform Light* uniform* uniform)lights;
    self->abstract.numLights = numLights;

    self->abstract.shadows = shadows;
    self->abstract.softShadows = softShadows;
    self->abstract.ambientOcclusionStrength = ambientOcclusionStrength;
    self->abstract.ambientOcclusionDistance = ambientOcclusionDistance;
}
