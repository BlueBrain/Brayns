/* Copyright 2015-2024 Blue Brain Project/EPFL
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

struct VoxelizedSimulationRenderer
{
    CircuitExplorerSimulationRenderer super;

    // Shading attributes
    float alphaCorrection;
    float simulationThreshold;
};

inline vec3f VoxelizedSimulationRenderer_shadeRay(
    const uniform VoxelizedSimulationRenderer* uniform self,
    varying ScreenSample& sample)
{
    Ray ray = sample.ray;
    ray.time = inf;
    sample.z = inf;

    float zDepth = 0.f;
    int depth = 0;
    vec3f normal;

    vec4f pathColor = make_vec4f(0.f);
    const uniform Material* previousMaterial = 0;

    while (pathColor.w < 1.f)
    {
        traceRay(self->super.super.super.model, ray);

        if (ray.geomID < 0)
        {
            // No intersection
            vec4f colorContribution =
                skyboxMapping((Renderer*)self, ray,
                              (DefaultMaterial*)self->super.super.bgMaterial);
            colorContribution.w = 1.f;
            composite(colorContribution, pathColor, self->alphaCorrection);
            break;
        }

        // Intersection found
        // Retreive information about the geometry, typically geometry ID,
        // normal to the surface, material ID, texture coordinates, etc.
        DifferentialGeometry dg;
        postIntersect(self->super.super.super.model, dg, ray,
                      DG_FACEFORWARD | DG_MATERIALID);

        // Material management
        const uniform Material* material = dg.material;

        if (material != previousMaterial)
        {
            const vec4f colorContribution =
                getSimulationValue(&self->super, &dg, ray.primID);

            if (colorContribution.w > self->simulationThreshold)
            {
                // Compositing according to alpha correction
                composite(colorContribution, pathColor, self->alphaCorrection);

                // Depth
                if (depth == 0)
                    sample.z = ray.t;

                ++depth;
            }
        }
        previousMaterial = material;

        // Next ray
        ray.t0 = max(0.f, ray.t + dg.epsilon);
        ray.t = inf;
        ray.primID = -1;
        ray.geomID = -1;
        ray.instID = -1;
    }

    // Alpha
    sample.alpha = pathColor.w;

    return make_vec3f(pathColor) * self->super.super.exposure;
}

void VoxelizedSimulationRenderer_renderSample(uniform Renderer* uniform _self,
                                              void* uniform perFrameData,
                                              varying ScreenSample& sample)
{
    uniform VoxelizedSimulationRenderer* uniform self =
        (uniform VoxelizedSimulationRenderer * uniform) _self;
    sample.rgb = VoxelizedSimulationRenderer_shadeRay(self, sample);
}

// Exports (called from C++)
export void* uniform VoxelizedSimulationRenderer_create(void* uniform cppE)
{
    uniform VoxelizedSimulationRenderer* uniform self =
        uniform new uniform VoxelizedSimulationRenderer;
    Renderer_Constructor(&self->super.super.super, cppE);
    self->super.super.super.renderSample =
        VoxelizedSimulationRenderer_renderSample;
    return self;
}

export void VoxelizedSimulationRenderer_set(
    void* uniform _self, void* uniform secondaryModel, void* uniform bgMaterial,
    const uniform uint32 spp, uniform float* uniform simulationData,
    const uniform uint64 simulationDataSize,
    const uniform float alphaCorrection,
    const uniform float simulationThreshold, const uniform float exposure,
    const uniform float fogThickness, const uniform float fogStart,
    const uniform uint32 maxBounces, const uniform bool useHardwareRandomizer)
{
    uniform VoxelizedSimulationRenderer* uniform self =
        (uniform VoxelizedSimulationRenderer * uniform) _self;

    // OSPRay
    self->super.super.super.spp = spp;

    // Abstract
    self->super.super.bgMaterial =
        (uniform CircuitExplorerMaterial * uniform) bgMaterial;

    self->super.super.lights = 0;
    self->super.super.numLights = 0;

    self->super.super.timestamp = 0.f;
    self->super.super.maxBounces = maxBounces;
    self->super.super.exposure = exposure;
    self->super.super.useHardwareRandomizer = useHardwareRandomizer;

    // Simulation
    self->super.simulationData = (uniform float* uniform)simulationData;
    self->super.simulationDataSize = simulationDataSize;

    self->super.secondaryModel = (uniform Model * uniform) secondaryModel;

    self->super.fogThickness = fogThickness;
    self->super.fogStart = fogStart;

    // Volexized simulation
    self->alphaCorrection = alphaCorrection;
    self->simulationThreshold = simulationThreshold;
}
