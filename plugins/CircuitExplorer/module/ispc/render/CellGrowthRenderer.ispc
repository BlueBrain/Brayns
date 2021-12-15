/* Copyright (c) 2015-2021, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
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

#include "utils/CircuitExplorerSimulationRenderer.ih"

struct CellGrowthRenderer
{
    CircuitExplorerSimulationRenderer super;

    // Shading attributes
    float alphaCorrection;
    float simulationThreshold;

    // Shading attributes
    int randomNumber;
    float shadows;
    float softShadows;
    float shadowDistance;

    // Shadow attributes

    // Use transfer function color for shading
    bool useTransferFunctionColor;
};

inline float processShadows(const uniform CellGrowthRenderer* uniform self,
                            varying ScreenSample& sample,
                            varying DifferentialGeometry& dg)

{
    // Shadows
    float shadowIntensity = 0.f;
    for (uniform int i = 0;
         shadowIntensity < 1.f && i < self->super.super.numLights; ++i)
    {
        const uniform Light* uniform light = self->super.super.lights[i];
        const vec2f samplingLocation = make_vec2f(0.5f);
        const varying Light_SampleRes lightSample =
            light->sample(light, dg, samplingLocation);
        const vec3f lightColor = lightSample.weight;
        const float radiance = reduce_max(lightColor);
        const vec3f lightDirection = lightSample.dir;

        if (radiance <= 0.f)
            continue;

        vec3f ld = lightDirection;
        if (self->softShadows > 0.f)
            ld = normalize(
                ld +
                self->softShadows *
                    getRandomVector(self->super.super.useHardwareRandomizer,
                                    self->super.super.super.fb->size.x, sample,
                                    dg.Ns, self->randomNumber));

        Ray shadowRay;
        setRay(shadowRay, dg.P, ld);
        shadowRay.t0 = dg.epsilon;
        shadowRay.t = self->shadowDistance;
        shadowRay.time = sample.ray.time;

        unsigned int iteration = 0;
        while (shadowIntensity < 1.f && iteration < NB_MAX_REBOUNDS)
        {
            ++iteration;

            shadowRay.geomID = -1;
            shadowRay.primID = -1;
            shadowRay.instID = -1;

            traceRay(self->super.super.super.model, shadowRay);

            if (shadowRay.geomID < 0)
                iteration = NB_MAX_REBOUNDS;
            else
            {
                DifferentialGeometry shadowDg;
                postIntersect(self->super.super.super.model, shadowDg,
                              shadowRay, DG_NG);
                const vec4f simulationColor =
                    getSimulationValue(&self->super, &shadowDg,
                                       shadowRay.primID);

                if (simulationColor.w > self->simulationThreshold)
                    shadowIntensity += self->shadows;

                shadowRay.t0 = shadowRay.t + shadowDg.epsilon;
                shadowRay.t = self->shadowDistance - shadowRay.t;
            }
        }
    }
    return shadowIntensity;
}

inline vec3f CellGrowthRenderer_shadeRay(
    const uniform CellGrowthRenderer* uniform self,
    varying ScreenSample& sample)
{
    Ray ray = sample.ray;
    ray.time = inf;
    sample.z = inf;

    float zDepth = 0.f;

    vec3f pathColor = make_vec3f(0.f);
    sample.alpha = 0.f;

    while (true)
    {
        vec3f Kd;
        float Ns = 1.f;

        traceRay(self->super.super.super.model, ray);

        if (ray.geomID < 0)
        {
            // No intersection
            vec4f colorContribution =
                skyboxMapping((Renderer*)self, ray,
                              (DefaultMaterial*)self->super.super.bgMaterial);
            colorContribution.w = 1.f;
            pathColor = make_vec3f(colorContribution);
            break;
        }

        // Intersection found
        // Retreive information about the geometry, typically geometry ID,
        // normal to the surface, material ID, texture coordinates, etc.
        DifferentialGeometry dg;
        postIntersect(self->super.super.super.model, dg, ray,
                      DG_NG | DG_NS | DG_NORMALIZE | DG_FACEFORWARD |
                          DG_MATERIALID | DG_COLOR);

        // Material management
        const uniform Material* material = dg.material;
        const uniform CircuitExplorerMaterial* objMaterial =
            (const uniform CircuitExplorerMaterial*)material;
        MaterialShadingMode shadingMode = diffuse;

        if (!objMaterial)
            Kd = make_vec3f(dg.color);
        else
            foreach_unique(mat in objMaterial)
            {
                shadingMode = mat->shadingMode;
                Kd = make_vec3f(dg.color) * mat->Kd;
                Ns = mat->Ns;
            }

        const vec4f simulationColor =
            getSimulationValue(&self->super, &dg, ray.primID);

        if (simulationColor.w > self->simulationThreshold)
        {
            if (self->useTransferFunctionColor)
                Kd = make_vec3f(simulationColor);

            // Shading
            const uniform Light* uniform light = self->super.super.lights[0];
            const vec2f samplingLocation = make_vec2f(0.5f);
            const varying Light_SampleRes lightSample =
                light->sample(light, dg, samplingLocation);
            const vec3f lightDirection = lightSample.dir;

            sample.z = ray.t;
            sample.alpha = 1.f;
            const vec3f intersection = ray.org + ray.t * ray.dir;

            float cosNL = 1.f;
            switch (shadingMode)
            {
            case electron:
            case electron_transparency:
                cosNL = 1.f - max(0.f, dot(neg(ray.dir), dg.Ns));
                break;
            case cartoon:
                cosNL = max(0.f, dot(lightDirection, dg.Ns));
                const unsigned int angleAsInt = cosNL * Ns;
                cosNL = (float)(angleAsInt) / Ns;
                break;
            case perlin:
                float n = noise(intersection * Ns);
                if (ray.t < dg.epsilon * 1.5e4f)
                    n += worleyNoise(intersection, 2.f);
                else
                    n *= 2.f;

                n = clamp(n, 0.f, 1.f);
                cosNL = n * max(0.f, dot(lightDirection, dg.Ns));
                break;
            case diffuse:
            case diffuse_transparency:
                cosNL = max(0.f, dot(lightDirection, dg.Ns));
            }

            float shadowIntensity = 0.f;
            if (self->shadows > 0.f)
                shadowIntensity = processShadows(self, sample, dg);

            pathColor = cosNL * Kd * clamp(1.f - shadowIntensity, 0.f, 1.f);
            break;
        }

        // Next ray
        ray.t0 = ray.t + dg.epsilon;
        ray.t = inf;
        ray.primID = -1;
        ray.geomID = -1;
        ray.instID = -1;
    }

    return make_vec3f(pathColor) * self->super.super.exposure;
}

void CellGrowthRenderer_renderSample(uniform Renderer* uniform _self,
                                     void* uniform perFrameData,
                                     varying ScreenSample& sample)
{
    uniform CellGrowthRenderer* uniform self =
        (uniform CellGrowthRenderer * uniform) _self;
    sample.rgb = CellGrowthRenderer_shadeRay(self, sample);
}

// Exports (called from C++)
export void* uniform CellGrowthRenderer_create(void* uniform cppE)
{
    uniform CellGrowthRenderer* uniform self =
        uniform new uniform CellGrowthRenderer;
    Renderer_Constructor(&self->super.super.super, cppE);
    self->super.super.super.renderSample = CellGrowthRenderer_renderSample;
    return self;
}

export void CellGrowthRenderer_set(
    void* uniform _self, void* uniform secondaryModel, void* uniform bgMaterial,
    const uniform int spp, void** uniform lights, const uniform int32 numLights,
    uniform float* uniform simulationData,
    const uniform int64 simulationDataSize, const uniform float alphaCorrection,
    const uniform float simulationThreshold, const uniform float exposure,
    const uniform float fogThickness, const uniform float fogStart,
    const uniform float shadows, const uniform float softShadows,
    const uniform float shadowDistance,
    const uniform bool useTransferFunctionColor,
    const uniform bool useHardwareRandomizer)
{
    uniform CellGrowthRenderer* uniform self =
        (uniform CellGrowthRenderer * uniform) _self;

    // OSPRay
    self->super.super.super.spp = spp;

    // Abstract
    self->super.super.bgMaterial =
        (uniform CircuitExplorerMaterial * uniform) bgMaterial;
    self->super.super.lights = (const uniform Light* uniform* uniform)lights;
    self->super.super.numLights = numLights;
    self->super.super.useHardwareRandomizer = useHardwareRandomizer;

    // Simulation
    self->super.secondaryModel = (uniform Model * uniform) secondaryModel;
    self->super.simulationData = (uniform float* uniform)simulationData;
    self->super.simulationDataSize = simulationDataSize;
    self->super.super.exposure = exposure;

    self->super.fogThickness = fogThickness;
    self->super.fogStart = fogStart;

    // Growth
    self->alphaCorrection = alphaCorrection;
    self->simulationThreshold = simulationThreshold;
    self->shadows = shadows;
    self->softShadows = softShadows;
    self->shadowDistance = shadowDistance;
    self->useTransferFunctionColor = useTransferFunctionColor;
}
