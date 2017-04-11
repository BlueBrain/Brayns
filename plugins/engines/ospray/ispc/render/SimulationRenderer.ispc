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

struct SimulationRenderer
{
    AbstractRenderer abstract;

    Model* simulationModel;
    uniform float* uniform simulationData;
    int64 simulationDataSize;
    float threshold;
};

/**
  The getSimulationValue reads simulation value from the simulation data buffer.
  The geometry contains the offset of the buffer in it's X texture coordinates.
  The value from the simulation data buffer is then converted into a color,
  according to the colormap.
  */
inline varying vec4f
    getSimulationValue(const uniform SimulationRenderer* uniform self,
                       DifferentialGeometry& dg, varying vec3f& lightEmission)
{
    lightEmission = make_vec3f(0.f);
    varying vec4f color = make_vec4f(0.f);

    varying float value = 0.f;
    if (dg.st.x >= 0 && dg.st.x < self->simulationDataSize)
        value = self->simulationData[dg.st.x];
    else
        // Value offset is out of range, return error color
        return make_vec4f(1.f, 0.f, 0.f, 1.f);

    if (value < self->threshold)
        return color;

    // Normalize the value according colormap size
    const varying float normalizedValue =
        self->abstract.colorMapSize *
        (value - self->abstract.colorMapMinValue) /
        self->abstract.colorMapRange;

    if (self->abstract.colorMap)
        color = self->abstract.colorMap[normalizedValue];

    if (self->abstract.emissionIntensitiesMap)
        lightEmission = self->abstract.emissionIntensitiesMap[normalizedValue];

    return color;
}

/**
  The purpose of the getValueFromSimulationModel function is to retreive the
  value of the simulation from a secondary model attached to the OSPRay scene.
  The geometry is defined by the primary model, but the shading is computed
  according to a second, non-visible model that contains the simulation values.
  In the case of circuit rendering for examples, neurons are meshes, and
  compartment reports are cones and cylinders that are inside the neuron. When
  an intersection is found at the surface of a mesh, a ray defined by the
  intersection point and the opposite normal to the surface is launched to hit
  the secondary model. When the secondary model is intersected, the simulation
  value is retrieved and used to shade the surface of the mesh.
*/
inline void getValueFromSimulationModel(
    const uniform SimulationRenderer* uniform self,
    varying ScreenSample& sample, const varying vec3f& intersection,
    const varying vec3f& normal, const int materialID,
    varying vec3f& simulationColor, varying float& lightEmission)
{
    // Get simulation color from simulation model
    varying Ray colorRay;
    colorRay.org = intersection;
    colorRay.dir = getRandomVector(sample, normal, self->abstract.randomNumber);
    colorRay.t0 = 0.f;
    colorRay.time = infinity;
    colorRay.t = infinity;
    colorRay.primID = -1;
    colorRay.geomID = -1;
    colorRay.instID = -1;

    // Trace ray to hit the simulation model
    traceRay(self->simulationModel, colorRay);

    if (colorRay.geomID >= 0)
    {
        // Intersection found. Simulation ray hit a primitive
        DifferentialGeometry colorDg;
        postIntersect(self->simulationModel, colorDg, colorRay,
                      DG_MATERIALID | DG_TEXCOORD);
        if (materialID == colorDg.materialID && self->simulationData)
        {
            // The mesh and it's corresponding representation in the
            // simulation model must use the same material ID. This is to
            // make sure that one neuron is not shaded with the simulation
            // value of another neuron.
            vec3f emission = make_vec3f(0.f);
            const vec4f color = getSimulationValue(self, colorDg, emission);
            simulationColor =
                simulationColor * (1.f - color.w) + make_vec3f(color) * color.w;
            lightEmission = reduce_max(emission);
        }
    }
    else
        // No intersection, color is initialized with the first value of the
        // colormap
        simulationColor = make_vec3f(self->abstract.colorMap[0]);
}

inline vec3f SimulationRenderer_shadeRay(
    const uniform SimulationRenderer* uniform self,
    varying ScreenSample& sample)
{
    Ray ray = sample.ray;
    varying float pathOpacity = 0.f;
    varying int depth = 0;
    varying float oldlocalRefraction = 1.f;

    varying float intersectionWeights[NB_MAX_REBOUNDS];
    varying vec3f intersectionColors[NB_MAX_REBOUNDS];
    varying vec3f specularColor = make_vec3f(0.f);
    varying float zDepth = 0.f;
    sample.z = 1.f;
    varying float tend = ray.t;

    varying bool moreRebounds = true;
    while (moreRebounds && depth < NB_MAX_REBOUNDS)
    {
        traceRay(self->abstract.super.model, ray);

        if (ray.geomID < 0)
        {
            // No intersection. Return volume contribution + skybox color
            ray.t = self->abstract.volumeDiag * 2.f;
            vec4f volumetricValue = make_vec4f(0.f);
            vec4f backgroundColor =
                skyboxMapping((Renderer*)self, ray, self->abstract.numMaterials,
                              self->abstract.materials);

            if (self->abstract.volumeData)
            {
                volumetricValue =
                    getVolumeContribution(&(self->abstract), ray, sample);
                if (depth == 0)
                    pathOpacity = 1.f - volumetricValue.w;
            }

            intersectionColors[depth] =
                volumetricValue.w * make_vec3f(volumetricValue) +
                (1.f - volumetricValue.w) * make_vec3f(backgroundColor);
            sample.z = 1.f - log(zDepth) / 10.f;
            moreRebounds = false;
        }
        else
        {
            // Retreive information about the geometry, typically geometry ID,
            // normal to the surface, material ID, texture coordinates, etc.
            DifferentialGeometry dg;
            postIntersect(self->abstract.super.model, dg, ray,
                          DG_NG | DG_NS | DG_NORMALIZE | DG_FACEFORWARD |
                              DG_MATERIALID | DG_COLOR | DG_TEXCOORD);

            // Initialisation for the current intersection
            varying float localLightEmission = 0.f;
            varying float localRefraction = 1.f;
            varying float localReflection = 0.f;
            varying float localOpacity = 0.f;
            varying vec3f localSimulationColor = make_vec3f(0.f);
            varying float localSimulationIntensity = 0.f;
            varying vec3f localDiffuseColor = make_vec3f(0.f);
            varying vec3f localSpecularColor = make_vec3f(0.f);
            varying float localSpecularExponent = 0.f;
            varying vec3f localNormal = dg.Ns;
            varying float localLightIntensity = 1.f;

            // Get simulation value from geometry ID if no simulation model
            // is used.
            varying vec3f lightEmission = make_vec3f(0.f);
            vec4f simulationValue = make_vec4f(0.f);
            if (!self->simulationModel && self->simulationData)
                simulationValue = getSimulationValue(self, dg, lightEmission);

            localSimulationColor = lightEmission + make_vec3f(simulationValue);
            localSimulationIntensity = simulationValue.w;
            localLightEmission = reduce_max(lightEmission);

            // localShadedColor defines the color for the current intersected
            // surface, and for the current ray generation only. This value is
            // reset and updated for every rebound
            varying vec3f localShadedColor = make_vec3f(0.f);

            // Get volumetric information
            if (self->abstract.volumeData)
            {
                vec4f volumetricValue =
                    getVolumeContribution(&(self->abstract), ray, sample);
                localOpacity = volumetricValue.w;
                intersectionColors[depth] = localShadedColor =
                    make_vec3f(volumetricValue);
                if (localOpacity >= 1.f)
                {
                    pathOpacity = 1.f;
                    break;
                }
            }

            // Process material attributes
            const uniform Material* material = dg.material;
            const uniform ExtendedOBJMaterial* objMaterial =
                (const uniform ExtendedOBJMaterial*)material;
            if (!objMaterial)
            {
                // Diffuse color
                localDiffuseColor = make_vec3f(dg.color);
                // if no material is defined, localOpacity is stored in the
                // alpha
                // component of the RGBA color
                localOpacity = dg.color.w;
            }
            else
            {
                foreach_unique(mat in objMaterial)
                {
                    // Light emission strength
                    localLightEmission += mat->a;

                    // Glossiness
                    localNormal = normalize(
                        localNormal +
                        mat->g * getRandomVector(sample, localNormal,
                                                 self->abstract.randomNumber));
                    // Refraction index
                    localRefraction = mat->refraction;

                    // Opacity
                    localOpacity += mat->d * get1f(mat->map_d, dg.st, 1.f);

                    // Diffuse color
                    localDiffuseColor = mat->Kd * make_vec3f(dg.color);
                    if (mat->map_Kd)
                    {
                        vec4f localDiffuseColorFromMap =
                            get4f(mat->map_Kd, dg.st);
                        localDiffuseColor =
                            localDiffuseColor *
                            make_vec3f(localDiffuseColorFromMap);
                        localOpacity *= localDiffuseColorFromMap.w;
                    }

                    // Specular color
                    localSpecularColor = max(localSpecularColor,
                                             mat->Ks * get3f(mat->map_Ks, dg.st,
                                                             make_vec3f(1.f)));

                    // Specular exponent
                    localSpecularExponent =
                        max(localSpecularExponent,
                            mat->Ns * get1f(mat->map_Ns, dg.st, 1.f));

                    // Normal mapping
                    if (mat->map_Normal)
                    {
                        vec3f n =
                            get3f(mat->map_Normal, dg.st, make_vec3f(0.f)) -
                            0.5f;
                        localNormal.x -= n.x;
                        localNormal.y -= n.y;
                    }

                    // localReflection index
                    localReflection = mat->reflection;
                    if (mat->map_reflection)
                    {
                        vec3f n =
                            get3f(mat->map_reflection, dg.st, make_vec3f(0.f));
                        localReflection *= (n.x + n.y + n.z) / 3.f;
                    }
                }
            }

            if (localOpacity > 0.01f)
            {
                intersectionWeights[depth] = 1.f;
                intersectionColors[depth] = make_vec3f(0.f);

                if (self->simulationModel)
                    // Get color from simulation model
                    getValueFromSimulationModel(self, sample, dg.P, neg(dg.Ng),
                                                dg.materialID,
                                                localDiffuseColor,
                                                localLightEmission);

                // Intersection point with geometry, slightly above the surface,
                // along the normal
                const varying vec3f intersection =
                    dg.P + self->abstract.super.epsilon * dg.Ng;

                // Indirect shading
                varying vec3f indirectShadingColor = make_vec3f(0.f);
                varying float indirectShadingIntensity = 0.f;
                const varying vec3f reflectedDirection =
                    ray.dir - 2.f * dot(ray.dir, localNormal) * localNormal;

                if (self->abstract.ambientOcclusionStrength != 0.f &&
                    localLightEmission == 0.f)
                {
                    // Indirect illumination
                    DifferentialGeometry geometry;
                    if (indirectShading(&(self->abstract), ray, sample,
                                        intersection, localNormal, geometry,
                                        indirectShadingColor,
                                        indirectShadingIntensity))
                    {
                        indirectShadingColor =
                            indirectShadingColor *
                            self->abstract.ambientOcclusionStrength;

                        indirectShadingIntensity *=
                            self->abstract.ambientOcclusionStrength;

                        // Get simulation value from geometry ID
                        varying vec3f lightEmission = make_vec3f(0.f);
                        vec4f simulationValue = make_vec4f(0.f);

                        if (self->simulationData)
                            simulationValue = getSimulationValue(self, geometry,
                                                                 lightEmission);

                        indirectShadingColor = indirectShadingColor +
                                               lightEmission +
                                               make_vec3f(simulationValue);

                        indirectShadingIntensity += reduce_max(lightEmission);
                    }
                }

                if (self->abstract.numLights == 0)
                {
                    // No lights defined
                    localLightIntensity = localLightEmission;
                    localLightIntensity =
                        max(localLightIntensity, indirectShadingIntensity);
                    localShadedColor = localShadedColor +
                                       localOpacity * (localDiffuseColor +
                                                       indirectShadingColor);
                }
                else if (self->abstract.electronShadingEnabled)
                {
                    // Electron Shading
                    localLightIntensity = 1.f;
                    const varying vec3f viewer =
                        normalize(ray.org - intersection);
                    const varying float el = max(0.f, dot(viewer, localNormal));
                    varying float cosNL = localLightEmission + (1.f - el);
                    localShadedColor =
                        localShadedColor +
                        localOpacity * cosNL *
                            (
                                // Affect shading with color determined by
                                // simulation value
                                (localSimulationColor *
                                 localSimulationIntensity) +
                                (1.f - localSimulationIntensity) *
                                    localDiffuseColor) +
                        indirectShadingColor * indirectShadingIntensity;
                }
                else
                {
                    // Shading for all lights defined in the scene
                    for (uniform int i = 0;
                         self->abstract.lights && i < self->abstract.numLights;
                         ++i)
                    {
                        const uniform Light* uniform light =
                            self->abstract.lights[i];
                        const varying vec2f s = make_vec2f(0.5f);
                        const varying Light_SampleRes lightSample =
                            light->sample(light, dg, s);
                        varying vec3f radiance =
                            lightSample.weight + indirectShadingIntensity;
                        varying vec3f lightDirection = lightSample.dir;

                        if (reduce_max(radiance) > 0.f)
                        {
                            varying float cosNL = 1.f;
                            varying float cosLR = 1.f;
                            // Phong + Blinn shading
                            if (self->abstract.shadingEnabled)
                            {
                                cosNL = min(1.f, max(0.f, dot(lightDirection,
                                                              localNormal)));
                                cosLR =
                                    min(1.f, max(0.f, dot(lightDirection,
                                                          reflectedDirection)));

                                // Specular color
                                specularColor =
                                    specularColor +
                                    localSpecularColor *
                                        powf(cosLR, localSpecularExponent);
                            }

                            cosNL += localLightEmission;
                            cosLR += localLightEmission;

                            // Process shadows
                            if (self->abstract.shadows != 0.f &&
                                localLightEmission == 0.f)
                                localLightIntensity =
                                    shadedLightIntensity(&(self->abstract), ray,
                                                         sample, intersection,
                                                         localNormal,
                                                         lightDirection);

                            // Diffuse color
                            const varying vec3f localUnshadedColor =
                                cosNL * localLightIntensity *
                                    (
                                        // Affect shading with color determined
                                        // by
                                        // simulation value
                                        (localSimulationColor *
                                         localSimulationIntensity) +
                                        (1.f - localSimulationIntensity) *
                                            localDiffuseColor) +
                                indirectShadingColor * indirectShadingIntensity;

                            // Add contribution of current light
                            localShadedColor =
                                localShadedColor * (1.f - localOpacity) +
                                localUnshadedColor * localOpacity;
                        }
                    }
                }

                // Store final color for current iteration
                intersectionColors[depth] =
                    intersectionColors[depth] +
                    min(make_vec3f(1.f), localShadedColor + specularColor);

                // Update cumulated path opacity
                pathOpacity += localOpacity;
                pathOpacity = min(1.f, pathOpacity);

                // Prepare ray for next iteration
                if (localOpacity < 1.f)
                {
                    const float angle = dot(ray.dir, localNormal);
                    if (localRefraction == oldlocalRefraction)
                        if (angle < 0.f)
                            // Set localRefraction to default index if exiting
                            // volume
                            localRefraction = 1.f;

                    ray.t0 = ray.t + self->abstract.super.epsilon;
                    ray.dir =
                        refractedVector(ray.dir, localNormal,
                                        oldlocalRefraction, localRefraction);

                    intersectionWeights[depth] = 1.f - localOpacity;
                    oldlocalRefraction = localRefraction;
                    ++depth;
                }
                else if (localReflection > 0.f)
                {
                    ray.dir = reflectedDirection;
                    ray.t0 = self->abstract.super.epsilon;
                    ray.org = intersection;
                    intersectionWeights[depth] = localReflection;
                    ++depth;
                }
                else
                    moreRebounds = false;
            }
            else
                // Fully transparent object. Ignore intersection
                ray.t0 = ray.t + 1.f;

            // Z-Depth
            zDepth += localOpacity * ray.t;
            sample.z = log(zDepth) / 10.f;

            // Next ray
            tend -= ray.t;
            if (tend < 0.f)
                break;
            ray.t = tend;
            ray.primID = -1;
            ray.geomID = -1;
            ray.instID = -1;
        }
    }

    // Z-Depth
    if (depth >= NB_MAX_REBOUNDS)
        sample.z = 1.f - log(zDepth) / 10.f;

    // Back to front computation of final color according to colors and weights
    // of each ray generation
    sample.alpha = pathOpacity;
    for (int i = depth - 2; i >= 0; --i)
        intersectionColors[i] =
            intersectionColors[i + 1] * intersectionWeights[i] +
            intersectionColors[i] * (1.f - intersectionWeights[i]);
    return intersectionColors[0];
}

void SimulationRenderer_renderSample(uniform Renderer* uniform _self,
                                     void* uniform perFrameData,
                                     varying ScreenSample& sample)
{
    uniform SimulationRenderer* uniform self =
        (uniform SimulationRenderer * uniform)_self;
    sample.ray.time = infinity;
    sample.rgb = SimulationRenderer_shadeRay(self, sample);
}

// Exports (called from C++)
export void* uniform SimulationRenderer_create(void* uniform cppE)
{
    uniform SimulationRenderer* uniform self =
        uniform new uniform SimulationRenderer;
    Renderer_Constructor(&self->abstract.super, cppE);
    self->abstract.super.renderSample = SimulationRenderer_renderSample;
    return self;
}

export void SimulationRenderer_set(
    void* uniform _self, void* uniform simulationModel,
    const uniform vec3f& bgColor, const uniform float& shadows,
    const uniform float& softShadows,
    const uniform float& ambientOcclusionStrength,
    const uniform bool& shadingEnabled, const uniform int& randomNumber,
    const uniform float& timestamp, const uniform int& spp,
    const uniform bool& electronShadingEnabled, void** uniform lights,
    const uniform int32 numLights, void** uniform materials,
    const uniform int32 numMaterials, uniform uint8* uniform volumeData,
    const uniform vec3i& volumeDimensions,
    const uniform vec3f& volumeElementSpacing,
    const uniform vec3f& volumeOffset, const uniform float& volumeEpsilon,
    uniform float* uniform simulationData,
    const uniform int64& simulationDataSize, uniform vec4f* uniform colormap,
    uniform vec3f* uniform emissionIntensitiesMap,
    const uniform int32 colorMapSize, const uniform float& colorMapMinValue,
    const uniform float& colorMapRange, const uniform float& threshold)
{
    uniform SimulationRenderer* uniform self =
        (uniform SimulationRenderer * uniform)_self;

    self->abstract.bgColor = bgColor;
    self->abstract.shadows = shadows;
    self->abstract.softShadows = softShadows;
    self->abstract.ambientOcclusionStrength = ambientOcclusionStrength;
    self->abstract.shadingEnabled = shadingEnabled;
    self->abstract.randomNumber = randomNumber;
    self->abstract.timestamp = timestamp;
    self->abstract.spp = spp;
    self->abstract.electronShadingEnabled = electronShadingEnabled;

    self->abstract.lights = (const uniform Light* uniform* uniform)lights;
    self->abstract.numLights = numLights;

    self->abstract.materials =
        (const uniform ExtendedOBJMaterial* uniform* uniform)materials;
    self->abstract.numMaterials = numMaterials;

    self->abstract.volumeData = (uniform uint8 * uniform)volumeData;
    self->abstract.volumeDimensions = volumeDimensions;
    self->abstract.volumeElementSpacing = volumeElementSpacing;
    self->abstract.volumeOffset = volumeOffset;
    self->abstract.volumeEpsilon = volumeEpsilon;

    const uniform vec3f diag =
        make_vec3f(volumeDimensions) * volumeElementSpacing;
    self->abstract.volumeDiag = max(diag.x, max(diag.y, diag.z));

    self->abstract.colorMap = (uniform vec4f * uniform)colormap;
    self->abstract.emissionIntensitiesMap =
        (uniform vec3f * uniform)emissionIntensitiesMap;
    self->abstract.colorMapSize = colorMapSize;
    self->abstract.colorMapMinValue = colorMapMinValue;
    self->abstract.colorMapRange = colorMapRange;

    self->simulationModel = (uniform Model * uniform)simulationModel;
    self->simulationData = (uniform float* uniform)simulationData;
    self->simulationDataSize = simulationDataSize;
    self->threshold = threshold;
}
