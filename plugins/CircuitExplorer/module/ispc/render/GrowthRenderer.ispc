/* Copyright (c) 2018, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of the circuit explorer for Brayns
 * <https://github.com/favreau/Brayns-UC-CircuitExplorer>
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

struct GrowthRenderer
{
    SimulationRenderer super;

    // Shading attributes
    float alphaCorrection;
    float simulationThreshold;

    // GI attributes
    float giStrength;
    float giDistance;
    int giSamples;
    float giSoftness;

    // Use transfer function color for shading
    bool useTransferFunctionColor;
};

inline vec3f GrowthRenderer_shadeRay(const uniform GrowthRenderer* uniform self,
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

        traceRay(self->super.super.super.model, ray);

        if (ray.geomID < 0)
        {
            // No intersection
            vec4f colorContribution =
                skyboxMapping((Renderer*)self, ray,
                              self->super.super.bgMaterial);
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
        const uniform ExtendedMaterial* objMaterial =
            (const uniform ExtendedMaterial*)material;
        MaterialShadingMode shadingMode = diffuse;

        if (!objMaterial)
            Kd = make_vec3f(dg.color);
        else
            foreach_unique(mat in objMaterial)
            {
                shadingMode = mat->shadingMode;
                Kd = make_vec3f(dg.color) * mat->Kd;
            }

        const vec4f colorContribution =
            getSimulationValue(&self->super, &dg, ray.primID);

        if (colorContribution.w > self->simulationThreshold)
        {
            if (self->useTransferFunctionColor)
                Kd = make_vec3f(colorContribution);

            // Light
            const uniform Light* uniform light = self->super.super.lights[0];
            const vec2f samplingLocation = make_vec2f(0.5f);
            const varying Light_SampleRes lightSample =
                light->sample(light, dg, samplingLocation);
            const vec3f lightDirection = lightSample.dir;

            sample.z = ray.t;
            sample.alpha = 1.f;
            const vec3f intersection = ray.org + ray.t * ray.dir;

            // Head-light shading
            float cosNL = 1.f;

            switch (shadingMode)
            {
            case electron:
            case electron_transparency:
                cosNL = 1.f - max(0.f, dot(neg(ray.dir), dg.Ns));
                break;
            case cartoon:
                cosNL = max(0.f, dot(lightDirection, dg.Ns));
                const unsigned int angleAsInt =
                    cosNL * DEFAULT_CARTOON_GRADIENT;
                cosNL = (float)(angleAsInt) / (float)(DEFAULT_CARTOON_GRADIENT);
                break;
            case perlin:
                float n = noise(intersection);
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

            // Ambient occlusion
            if (self->giSamples != 0)
            {
                unsigned int count = 0;
                for (int i = 0; i < self->giSamples; ++i)
                {
                    // AO
                    vec3f randomDirection = normalize(
                        lightDirection +
                        self->giSoftness *
                            getRandomVector(self->super.super.super.fb->size.x,
                                            sample, dg.Ns, i));
                    if (dot(randomDirection, dg.Ns) < 0.f)
                        // Invert direction of random ray direction is opposite
                        // to surface normal
                        randomDirection = neg(randomDirection);

                    varying Ray randomRay = sample.ray;
                    setRay(randomRay, intersection, randomDirection);
                    randomRay.t0 = dg.epsilon;
                    randomRay.t = self->giDistance;
                    randomRay.primID = -1;
                    randomRay.geomID = -1;
                    randomRay.instID = -1;

                    traceRay(self->super.super.super.model, randomRay);

                    if (randomRay.geomID >= 0)
                    {
                        DifferentialGeometry randomDg;
                        postIntersect(self->super.super.super.model, randomDg,
                                      randomRay, DG_NG);

                        const vec4f colorContribution =
                            getSimulationValue(&self->super, &randomDg,
                                               randomRay.primID);

                        if (colorContribution.w > self->simulationThreshold)
                            ++count;
                    }
                }
                if (count > 0)
                {
                    cosNL -= (float)count * self->giStrength;
                    cosNL = max(0.f, cosNL);
                }
            }

            pathColor = cosNL * Kd;
            break;
        }

        // Next ray
        ray.t0 = ray.t + dg.epsilon;
        ray.t = inf;
        ray.primID = -1;
        ray.geomID = -1;
        ray.instID = -1;
    }

    return make_vec3f(pathColor) * self->super.pixelAlpha;
}

void GrowthRenderer_renderSample(uniform Renderer* uniform _self,
                                 void* uniform perFrameData,
                                 varying ScreenSample& sample)
{
    uniform GrowthRenderer* uniform self =
        (uniform GrowthRenderer * uniform) _self;
    sample.rgb = GrowthRenderer_shadeRay(self, sample);
}

// Exports (called from C++)
export void* uniform GrowthRenderer_create(void* uniform cppE)
{
    uniform GrowthRenderer* uniform self = uniform new uniform GrowthRenderer;
    Renderer_Constructor(&self->super.super.super, cppE);
    self->super.super.super.renderSample = GrowthRenderer_renderSample;
    return self;
}

export void GrowthRenderer_set(
    void* uniform _self, void* uniform secondaryModel, void* uniform bgMaterial,
    const uniform int& spp, void** uniform lights,
    const uniform int32 numLights, uniform float* uniform simulationData,
    const uniform int64 simulationDataSize,
    const uniform float& alphaCorrection,
    const uniform float& simulationThreshold, const uniform float& pixelAlpha,
    const uniform float& fogThickness, const uniform float& fogStart,
    const uniform float& giStrength, const uniform float& giDistance,
    const uniform int& giSamples, const uniform float& giSoftness,
    const uniform bool& useTransferFunctionColor)
{
    uniform GrowthRenderer* uniform self =
        (uniform GrowthRenderer * uniform) _self;

    self->super.secondaryModel = (uniform Model * uniform) secondaryModel;
    self->super.super.bgMaterial =
        (uniform ExtendedMaterial * uniform) bgMaterial;

    self->super.super.super.spp = spp;

    self->super.super.lights = (const uniform Light* uniform* uniform)lights;
    self->super.super.numLights = numLights;

    self->super.simulationData = (uniform float* uniform)simulationData;
    self->super.simulationDataSize = simulationDataSize;

    self->alphaCorrection = alphaCorrection;
    self->simulationThreshold = simulationThreshold;

    self->super.pixelAlpha = pixelAlpha;

    self->super.fogThickness = fogThickness;
    self->super.fogStart = fogStart;

    self->giStrength = giStrength;
    self->giDistance = giDistance;
    self->giSamples = giSamples;
    self->giSoftness = giSoftness;

    self->useTransferFunctionColor = useTransferFunctionColor;
}
