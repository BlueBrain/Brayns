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
#include "utils/AbstractRenderer.ih"
#include <ospray/SDK/common/Ray.ih>
#include <ospray/SDK/render/util.ih>

struct PathTracingRenderer
{
    AbstractRenderer super;

    float shadows;
    float softShadows;
};

inline vec3f mix(const vec3f a, const vec3f b, float t)
{
    const float ax = a.x, ay = a.y, az = a.z;
    return make_vec3f(ax + t * (b.x - ax), ay + t * (b.y - ay),
                      az + t * (b.z - az));
}

inline float rotate(float x, float dx)
{
    x += dx;
    if (x >= 1.f) // keep x always between [0.0-1.0]
        x -= 1.f;
    return x;
}

inline void getBinormals(vec3f& biNorm0, vec3f& biNorm1, const vec3f& gNormal)

{
    biNorm0 = make_vec3f(1.f, 0.f, 0.f);
    if (abs(dot(biNorm0, gNormal)) > .95f)
        biNorm0 = make_vec3f(0.f, 1.f, 0.f);

    biNorm1 = normalize(cross(biNorm0, gNormal));
    biNorm0 = normalize(cross(biNorm1, gNormal));
}

// gets a random direction from a rotated frame
inline vec3f getRandomDir(varying RandomTEA* uniform rng, const vec3f biNorm0,
                          const vec3f biNorm1, const vec3f gNormal,
                          const float rot_x, const float rot_y,
                          const float epsilon)

{
    const vec2f rn = RandomTEA__getFloats(rng);
    const float r0 = rotate(rn.x, rot_x);
    const float r1 = rotate(rn.y, rot_y);
    const float w = sqrt(1.f - r1);
    const float x = cos((2.f * M_PI) * r0) * w;
    const float y = sin((2.f * M_PI) * r0) * w;
    const float z = sqrt(r1) + epsilon;
    return x * biNorm0 + y * biNorm1 + z * gNormal;
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
    vec3f color = make_vec3f(0.f);
    vec3f accucolor = make_vec3f(0.f);
    vec3f mask = make_vec3f(1.f);

    sample.z = inf;
    sample.alpha = 0.f;

    traceRay(self->super.super.model, ray);

    if (ray.geomID < 0)
    {
        // No intersection. Return skybox color
        sample.alpha = 0.f;
        return make_vec3f(skyboxMapping((Renderer*)self, ray,
                                        self->super.bgMaterial)) *
               DEFAULT_SKY_POWER_ZERO_BOUNCE;
    }

    // else, we've got a hit!
    DifferentialGeometry dg;
    postIntersect(self->super.super.model, dg, ray,
                  DG_NS | DG_NG | DG_NORMALIZE | DG_FACEFORWARD |
                      DG_MATERIALID | DG_COLOR);

    uniform Material* material = dg.material;
    uniform DefaultMaterial* objMaterial =
        (uniform DefaultMaterial*)material;

    vec3f Kd = make_vec3f(0.f);
    if (!objMaterial)
        Kd = make_vec3f(dg.color);
    else
        foreach_unique(mat in objMaterial) Kd = mat->Kd * make_vec3f(dg.color);

    // some stuff about RNG like Halton
    const uniform int accumID =
        reduce_max(sample.sampleID.z) * self->super.super.spp;
    const uniform float rot_x = 1.f - precomputedHalton3(accumID);
    const uniform float rot_y = 1.f - precomputedHalton5(accumID);

    // more RNG stuff
    uniform FrameBuffer* uniform fb = self->super.super.fb;
    RandomTEA rng_state;
    varying RandomTEA* const uniform rng = &rng_state;
    RandomTEA__Constructor(rng,
                           (fb->size.x * sample.sampleID.y) + sample.sampleID.x,
                           accumID);

    // path tracing loop
    Ray localray = ray;
    for (int bounces = 0; bounces < NB_MAX_PATH_TRACING_REBOUNDS; bounces++)
    {
        traceRay(self->super.super.model, localray);

        // if ray misses scene (no hit occurs), return background colour
        if (localray.geomID < 0)
        {
            const vec3f bgcol =
                make_vec3f(skyboxMapping((Renderer*)self, localray,
                                         self->super.bgMaterial)) *
                DEFAULT_SKY_POWER_ZERO_BOUNCE;

            accucolor = accucolor + mask * bgcol;
            break;
        }
        DifferentialGeometry dg;
        postIntersect(self->super.super.model, dg, localray,
                      DG_NG | DG_NS | DG_NORMALIZE | DG_FACEFORWARD |
                          DG_MATERIALID | DG_COLOR);

        const float epsilon = dg.epsilon;
        uniform Material* material = dg.material;
        uniform DefaultMaterial* objMaterial =
            (uniform DefaultMaterial*)material;
        if (!objMaterial)
            Kd = make_vec3f(dg.color);
        else
            foreach_unique(mat in objMaterial) Kd =
                mat->Kd * make_vec3f(dg.color);

        // compute ray direction of uniformly random diffuse ray
        const vec3f N = dg.Ns;
        vec3f biNormU = make_vec3f(1, 1, 1);
        vec3f biNormV = make_vec3f(1, 1, 1);
        getBinormals(biNormU, biNormV, N);
        const vec3f raydir =
            getRandomDir(rng, biNormU, biNormV, N, rot_x, rot_y, epsilon);

        // origin of new ray in path is hitpoint of previous ray in path
        const vec3f hitpoint = dg.P + epsilon * dg.Ns;
        setRay(localray, hitpoint, raydir);
        localray.t0 = epsilon;
        localray.t = inf;

        if (self->shadows > 0.f)
        {
            // Direct lighting
            for (uniform int i = 0;
                 self->super.lights && i < self->super.numLights; ++i)
            {
                const uniform Light* uniform light = self->super.lights[i];
                const vec2f s = make_vec2f(0.5f);
                const varying Light_SampleRes lightSample =
                    light->sample(light, dg, s);
                const vec3f radiance = lightSample.weight;

                if (reduce_max(radiance) > 0.f)
                {
                    const vec3f lightDirection = lightSample.dir;
                    const vec3f lightSampleDirection =
                        getConeSample(lightDirection, rng, self->softShadows);
                    const float cosNL = dot(dg.Ns, lightSampleDirection);

                    if (cosNL > 0.f) // if surface is facing light
                    {
                        // create and trace a shadowray towards the sun
                        Ray lightRay;
                        setRay(lightRay, hitpoint, lightSampleDirection);
                        if (!isOccluded(self->super.super.model, lightRay))
                            // take surface color at this bounce's hitpoint into
                            // account
                            accucolor =
                                accucolor + mask * Kd * cosNL * radiance;
                    }
                }
            }
        }
        mask = mask * Kd;
    } // end bounces

    // Z-Depth
    sample.z = ray.t;
    sample.alpha = 1.f;

    return accucolor;
}

void PathTracingRenderer_renderSample(uniform Renderer* uniform _self,
                                      void* uniform perFrameData,
                                      varying ScreenSample& sample)
{
    uniform PathTracingRenderer* uniform self =
        (uniform PathTracingRenderer * uniform)_self;
    sample.ray.time = self->super.timestamp;
    sample.rgb = PathTracingRenderer_shadeRay(self, sample);
}

// Exports (called from C++)
export void* uniform PathTracingRenderer_create(void* uniform cppE)
{
    uniform PathTracingRenderer* uniform self =
        uniform new uniform PathTracingRenderer;
    Renderer_Constructor(&self->super.super, cppE);
    self->super.super.renderSample = PathTracingRenderer_renderSample;
    return self;
}

export void PathTracingRenderer_set(
    void* uniform _self, void* uniform bgMaterial,
    const uniform float& timestamp, const uniform int& spp,
    void** uniform lights, uniform int32 numLights,
    const uniform float& shadows, const uniform float& softShadows)
{
    uniform PathTracingRenderer* uniform self =
        (uniform PathTracingRenderer * uniform)_self;

    self->super.bgMaterial = (uniform DefaultMaterial * uniform)bgMaterial;
    self->super.timestamp = timestamp;
    self->super.super.spp = spp;

    self->super.lights = (const uniform Light* uniform* uniform)lights;
    self->super.numLights = numLights;

    self->shadows = shadows;
    self->softShadows = softShadows;
}
