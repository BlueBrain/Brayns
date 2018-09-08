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
#include "utils/SimulationRenderer.ih"

struct AdvancedSimulationRenderer
{
    SimulationRenderer super;

    // Shading attributes
    int randomNumber;
    float shadows;
    float softShadows;
    float ambientOcclusionStrength;
    float ambientOcclusionDistance;

    // Simulation model
    Model* simulationModel;
    float detectionDistance;

    // Volumes
    uint32 volumeSamplesPerRay;
    float samplingThreshold;
    float volumeSpecularExponent;
    float volumeAlphaCorrection;
};

struct ShadingAttributes
{
    const uniform AdvancedSimulationRenderer* uniform self;
    varying DifferentialGeometry* dg;
    vec3f origin;
    vec3f normal;
    vec3f reflectedNormal;
    float refraction;
    float reflection;
    float opacity;
    vec3f diffuseColor;
    vec3f specularColor;
    float specularExponent;
    vec3f lightEmissionColor;
    float lightEmissionIntensity;
    float shadowIntensity;
    bool castSimulationData;
    MaterialShadingMode shadingMode;
    vec3f simulationColor;
    float simulationIntensity;
    vec3f indirectColor;
    float indirectIntensity;
    vec3f volumeColor;
    float volumeIntensity;
    vec3f shadingContribution;
    vec3f finalContribution;
};

inline bool launchRandomRay(
    const uniform AdvancedSimulationRenderer* uniform self,
    varying ScreenSample& sample, const varying vec3f& intersection,
    const varying vec3f& normal, DifferentialGeometry& geometry,
    varying vec3f& backgroundColor, varying float& distanceToIntersection,
    varying vec3f& randomDirection)
{
    randomDirection = getRandomVector(self->super.super.super.fb->size.x,
                                      sample, normal, self->randomNumber);
    backgroundColor = make_vec3f(0.f);

    if (dot(randomDirection, normal) < 0.f)
        // Invert direction of random ray direction is opposite to surface
        // normal
        randomDirection = neg(randomDirection);

    varying Ray randomRay = sample.ray;
    setRay(randomRay, intersection, randomDirection);
    randomRay.t0 = geometry.epsilon;
    randomRay.t = self->ambientOcclusionDistance;
    randomRay.primID = -1;
    randomRay.geomID = -1;
    randomRay.instID = -1;

    traceRay(self->super.super.super.model, randomRay);

    if (randomRay.geomID < 0)
    {
        // No intersection
        backgroundColor =
            make_vec3f(skyboxMapping((Renderer*)self, randomRay,
                                     self->super.super.bgMaterial));
        return false;
    }

    // Random ray hits a primitive
    distanceToIntersection = randomRay.t * randomRay.t;
    postIntersect(self->super.super.super.model, geometry, randomRay,
                  DG_NG | DG_NS | DG_NORMALIZE | DG_MATERIALID | DG_COLOR |
                      DG_TEXCOORD);
    return true;
}

inline void indirectShading(
    const uniform AdvancedSimulationRenderer* uniform self,
    varying ScreenSample& sample, const varying vec3f& intersection,
    const varying vec3f& normal, DifferentialGeometry& geometry,
    varying vec3f& indirectShadingColor, varying float& indirectShadingPower)
{
    vec3f backgroundColor = make_vec3f(0.f);
    float distanceToIntersection = infinity;
    indirectShadingPower = 0.f;

    // Launch a random ray
    vec3f randomDirection;
    if (launchRandomRay(self, sample, intersection, normal, geometry,
                        backgroundColor, distanceToIntersection,
                        randomDirection))
    {
        // Determine material of intersected geometry
        const uniform Material* material = geometry.material;
        const uniform ExtendedOBJMaterial* objMaterial =
            (const uniform ExtendedOBJMaterial*)material;
        if (objMaterial)
            foreach_unique(mat in objMaterial)
            {
                // Check if material emits light and is random rays hits the
                // surface in the opposite direction from the surface normal
                if (mat->a != 0.f && dot(geometry.Ns, randomDirection) < 0.f)
                {
                    // Light emitting material
                    vec3f localDiffuseColor = mat->Kd;
                    if (valid(mat->map_Kd))
                        localDiffuseColor = get3f(mat->map_Kd, geometry.st);

                    // intensity
                    vec3f Ka = make_vec3f(mat->a);
                    indirectShadingPower = mat->a;
                    if (valid(mat->map_a))
                    {
                        Ka = get3f(mat->map_a, geometry.st);
                        indirectShadingPower = reduce_max(Ka);
                    }

                    // Returned color is the one material with weight according
                    // intensity and distance to intersection
                    indirectShadingColor = localDiffuseColor * Ka;
                    indirectShadingPower = 1.f / distanceToIntersection;
                }
                else
                {
                    // Material does not emit light, occlusion is considered
                    // according to surface localOpacity
                    indirectShadingColor = make_vec3f(1.f);
                    indirectShadingPower = -mat->d;
                }
            }
        else
        {
            // Material does not emit light, occlusion is considered
            // according to surface localOpacity
            indirectShadingColor = make_vec3f(geometry.color.w);
            indirectShadingPower = -geometry.color.w;
        }

        indirectShadingPower *= abs(dot(normal, randomDirection));
    }
    else
    {
        indirectShadingColor = backgroundColor;
        indirectShadingPower = DEFAULT_SKY_POWER;
    }
    indirectShadingPower *= self->ambientOcclusionStrength;
}

inline float getVolumeShadowContribution(
    Volume* uniform volume,
    const uniform AdvancedSimulationRenderer* uniform self,
    const varying Ray& ray, varying ScreenSample& sample)
{
    // Find volume intersections
    float t0, t1;
    intersectBox(ray, volume->boundingBox, t0, t1);

    // Ray marching from light source to voxel
    float shadowIntensity = 0.f;
    const float epsilon = volume->samplingStep / volume->samplingRate;
    for (float t = t1; t > epsilon && shadowIntensity < 1.f; t -= epsilon)
    {
        const vec3f point = ray.org + ray.dir * t;
        const float sample = volume->sample(volume, point);

        // Look up the opacity associated with the volume sample.
        shadowIntensity += volume->transferFunction->getOpacityForValue(
            volume->transferFunction, sample);
    }
    return shadowIntensity;
}

inline float getVolumeShadowContributions(
    Volume* uniform volume,
    const uniform AdvancedSimulationRenderer* uniform self,
    const varying Ray& ray, varying ScreenSample& sample, const vec3f& point,
    const float epsilon)
{
    float shadowIntensity = 0.f;
    for (uniform int i = 0; shadowIntensity < 1.f && self->super.super.lights &&
                            i < self->super.super.numLights;
         ++i)
    {
        const uniform Light* uniform light = self->super.super.lights[i];
        const varying vec2f s = make_vec2f(0.5f);
        DifferentialGeometry dg;
        dg.P = point;
        const varying Light_SampleRes lightSample = light->sample(light, dg, s);

        Ray lightRay = ray;
        lightRay.t = inf;
        lightRay.t0 = epsilon;
        lightRay.org = point;
        lightRay.geomID = -1;

        if (self->softShadows > 0.f)
            lightRay.dir = normalize(
                lightSample.dir +
                self->softShadows *
                    getRandomVector(self->super.super.super.fb->size.x, sample,
                                    lightSample.dir, self->randomNumber));
        else
            lightRay.dir = lightSample.dir;

        // Intersection with Geometry

        traceRay(self->super.super.super.model, lightRay);
        if (lightRay.geomID != -1)
        {
            shadowIntensity += 1.f;
            break;
        }

        // Intersection with volume
        shadowIntensity +=
            getVolumeShadowContribution(volume, self, lightRay, sample);
    }
    return shadowIntensity * self->shadows;
}

inline vec4f getVolumeContribution(
    Volume* uniform volume,
    const uniform AdvancedSimulationRenderer* uniform self,
    const varying Ray& ray, varying ScreenSample& sample)
{
    // Find volume intersections
    float t0, t1;
    intersectBox(ray, volume->boundingBox, t0, t1);
    t1 = min(ray.t, t1);

    vec4f pathColor = make_vec4f(0.f);
    float epsilon = volume->samplingStep;
    float shadowIntensity = 0.f;

    // Introduce a bit of randomness to smooth the shading
    t0 -= getRandomValue(sample, self->randomNumber) * ((t1 - t0) * 0.01f);

    // Ray marching
    unsigned int shadingOccurence = 0;
    for (float t = t0; t < t1 && pathColor.w < 1.f; t += epsilon)
    {
        const vec3f point = ray.org + t * ray.dir;
        const float volumeSample = volume->sample(volume, point);

        // Look up the opacity associated with the volume sample
        const float sampleOpacity =
            volume->transferFunction->getOpacityForValue(
                volume->transferFunction, volumeSample);

        if (sampleOpacity <= self->samplingThreshold)
            // Continue walking for as long as voxel opacity is below sampling
            // threshold
            continue;

        // Adapt sampling rate to shading occurence
        epsilon =
            (volume->samplingStep + shadingOccurence) / volume->samplingRate;

        // Look up the color associated with the volume sample
        vec3f volumeSampleColor =
            volume->transferFunction->getColorForValue(volume->transferFunction,
                                                       volumeSample);

        // Voxel shading
        const bool firstShadingOccurence = shadingOccurence == 0;
        const bool shade = volume->singleShade ? firstShadingOccurence : true;

        vec3f gradient;
        DifferentialGeometry dg;
        const bool shadingEnabled = (shade && volume->gradientShadingEnabled);

        const bool aoEnabled =
            (firstShadingOccurence && self->ambientOcclusionStrength > 0.f);

        const bool shadowsEnabled =
            (firstShadingOccurence && self->shadows > 0.f);

        if (shadingEnabled || aoEnabled)
        {
            gradient = safe_normalize(volume->computeGradient(volume, point));
            // Setup differential geometry for the volume sample point.
            dg.P = point;
            dg.epsilon = epsilon;
            dg.Ns = gradient;
        }

        if (shadingEnabled)
        {
            // Compute light contributions
            vec3f shadedColor = make_vec3f(0.f);
            for (uniform int i = 0; i < self->super.super.numLights; ++i)
            {
                const vec2f samplingLocation = make_vec2f(0.5f);
                const Light_SampleRes light =
                    self->super.super.lights[i]->sample(
                        self->super.super.lights[i], dg, samplingLocation);
                // Diffuse
                const float cosNL =
                    (gradient.x == 0.f && gradient.y == 0.f &&
                     gradient.z == 0.f)
                        ? 1.f
                        : abs(dot(safe_normalize(light.dir), gradient));
                shadedColor = clamp(shadedColor + volumeSampleColor * cosNL *
                                                      light.weight,
                                    make_vec3f(0.f), make_vec3f(1.f));

                // Specular
                const vec3f reflectedNormal = normalize(
                    ray.dir - 2.f * dot(ray.dir, gradient) * gradient);
                const float cosNS =
                    powf(max(0.f, dot(light.dir, reflectedNormal)),
                         self->volumeSpecularExponent);
                shadedColor = shadedColor + volume->specular * cosNS;
            }
            volumeSampleColor = shadedColor;
        }

        if (aoEnabled)
        {
            // Compute ambient occlusion contribution
            vec3f indirectColor;
            float indirectIntensity;
            indirectShading(self, sample, point, gradient, dg, indirectColor,
                            indirectIntensity);
            volumeSampleColor = volumeSampleColor + sampleOpacity *
                                                        indirectColor *
                                                        indirectIntensity;
        }

        if (shadowsEnabled)
            // Compute shadow contribution
            shadowIntensity =
                getVolumeShadowContributions(volume, self, ray, sample, point,
                                             epsilon);

        // Compose color with according alpha correction
        composite(make_vec4f(volumeSampleColor, sampleOpacity), pathColor,
                  self->volumeAlphaCorrection * epsilon);

        ++shadingOccurence;
    }

    // Apply shadow to RGB values only
    return pathColor * make_vec4f(make_vec3f(1.f - shadowIntensity), 1.f);
}

inline float shadedLightIntensity(varying ScreenSample& sample,
                                  const varying Ray& ray,
                                  const ShadingAttributes& attributes,
                                  const varying vec3f& lightDirection,
                                  DifferentialGeometry& dg)
{
    float shadowIntensity = 0.f;

    vec3f ld = lightDirection;
    if (attributes.self->softShadows != 0.f)
        // Slightly alter light direction for Soft shadows
        ld = normalize(
            ld +
            attributes.self->softShadows *
                getRandomVector(attributes.self->super.super.super.fb->size.x,
                                sample, attributes.normal,
                                attributes.self->randomNumber));

    Ray shadowRay = ray;
    setRay(shadowRay, dg.P, ld);
    shadowRay.t0 = dg.epsilon;
    shadowRay.t = inf;
    shadowRay.time = sample.ray.time;
    shadowRay.geomID = -1;

    while (shadowIntensity < 1.f)
    {
        traceRay(attributes.self->super.super.super.model, shadowRay);

        if (shadowRay.geomID == -1)
            break;

        DifferentialGeometry shadowingGeometry;
        postIntersect(attributes.self->super.super.super.model,
                      shadowingGeometry, shadowRay,
                      DG_MATERIALID | DG_TEXCOORD | DG_COLOR);

        uniform ExtendedOBJMaterial* objMaterial =
            (uniform ExtendedOBJMaterial*)shadowingGeometry.material;

        float opacity = 1.f;

        if (objMaterial == 0)
            opacity = 1.f - shadowingGeometry.color.w;
        else
            foreach_unique(mat in objMaterial)
            {
                opacity = mat->d;
                if (valid(mat->map_d))
                {
                    vec4f d_map = get4f(mat->map_d, shadowingGeometry.st);
                    opacity = d_map.x;
                }
                else if (valid(mat->map_Kd))
                {
                    vec4f localDiffuseColor_map =
                        get4f(mat->map_Kd, shadowingGeometry.st);
                    opacity = 1.f - localDiffuseColor_map.w;
                }
            }

        shadowIntensity += opacity;

        shadowRay.t0 = shadowRay.t + shadowingGeometry.epsilon;
        shadowRay.t = inf;
        shadowRay.primID = -1;
        shadowRay.geomID = -1;
        shadowRay.instID = -1;
    }

    // Volume shadows
    shadowRay.org = dg.P;
    shadowRay.t0 = dg.epsilon;

    for (uniform int32 i = 0;
         shadowIntensity < 1.f &&
         i < attributes.self->super.super.super.model->volumeCount;
         ++i)
    {
        Volume* uniform volume =
            attributes.self->super.super.super.model->volumes[i];

        shadowIntensity += getVolumeShadowContribution(volume, attributes.self,
                                                       shadowRay, sample) *
                           attributes.self->shadows;
    }
    return clamp(shadowIntensity * attributes.self->shadows, 0.f, 1.f);
}

inline void initializeShadingAttributes(
    const uniform AdvancedSimulationRenderer* uniform self,
    ShadingAttributes& attributes)
{
    attributes.self = self;
    attributes.dg = 0;

    // Final contribution
    attributes.finalContribution = make_vec3f(0.f);

    // Simulation contribution
    attributes.castSimulationData = true;
    attributes.simulationColor = make_vec3f(0.f);
    attributes.simulationIntensity = 0.f;

    // Indirect light contribution
    attributes.indirectColor = make_vec3f(0.f);
    attributes.indirectIntensity = 0.f;

    // Light emission
    attributes.lightEmissionColor = make_vec3f(0.f);
    attributes.lightEmissionIntensity = 0.f;

    // Shadows
    attributes.shadowIntensity = 0.f;

    // Volume
    attributes.volumeColor = make_vec3f(0.f);
    attributes.volumeIntensity = 0.f;

    // Diffuse color
    attributes.diffuseColor = make_vec3f(0.f);

    // Specular color
    attributes.specularColor = make_vec3f(0.f);
    attributes.specularExponent = 0.f;

    // Shading contribution
    attributes.shadingContribution = make_vec3f(0.f);

    // Other attributes
    attributes.reflection = 0.f;
    attributes.refraction = 1.f;
    attributes.opacity = 0.f;
}

inline void setGeometryShadingAttributes(
    const uniform AdvancedSimulationRenderer* uniform self,
    DifferentialGeometry& dg, varying ScreenSample& sample,
    const varying Ray& ray, ShadingAttributes& attributes)
{
    attributes.dg = &dg;

    // Intersection point with geometry
    attributes.origin = dg.P;

    // Shading normal
    attributes.normal = dg.Ns;

    // Shading mode
    attributes.shadingMode = none;

    // Reflected shading normal
    attributes.reflectedNormal =
        ray.dir - 2.f * dot(ray.dir, attributes.normal) * attributes.normal;

    // Process material attributes
    const uniform Material* material = dg.material;
    const uniform ExtendedOBJMaterial* objMaterial =
        (const uniform ExtendedOBJMaterial*)material;
    if (!objMaterial)
    {
        // Diffuse color
        attributes.diffuseColor = make_vec3f(dg.color);

        // if no material is defined, localOpacity is stored in the alpha
        // component of the RGBA color
        attributes.opacity = dg.color.w;
        return;
    }

    foreach_unique(mat in objMaterial)
    {
        // Glossiness
        if (mat->glossiness < 1.f)
        {
            const vec3f randomNormal =
                (1.f - mat->glossiness) *
                getRandomVector(self->super.super.super.fb->size.x, sample,
                                attributes.normal, self->randomNumber);
            attributes.normal = normalize(attributes.normal + randomNormal);
        }

        // Refraction index
        attributes.refraction = mat->refraction;

        // Opacity
        if (valid(mat->map_d))
            attributes.opacity = mat->d * get1f(mat->map_d, dg.st);
        else
            attributes.opacity = mat->d;

        // Diffuse color
        attributes.diffuseColor = mat->Kd * make_vec3f(dg.color);
        if (valid(mat->map_Kd))
        {
            const vec4f diffuseColorFromMap = get4f(mat->map_Kd, dg.st);
            attributes.diffuseColor =
                attributes.diffuseColor * make_vec3f(diffuseColorFromMap);
            attributes.opacity *= diffuseColorFromMap.w;
        }

        // Specular color
        if (valid(mat->map_Ks))
            attributes.specularColor = max(attributes.specularColor,
                                           mat->Ks * get3f(mat->map_Ks, dg.st));
        else
            attributes.specularColor = max(attributes.specularColor, mat->Ks);

        // Specular exponent
        if (valid(mat->map_Ns))
            attributes.specularExponent =
                max(attributes.specularExponent,
                    mat->Ns * get1f(mat->map_Ns, dg.st));
        else
            attributes.specularExponent =
                max(attributes.specularExponent, mat->Ns);

        // Light emission intensity
        attributes.lightEmissionIntensity += mat->a;
        attributes.lightEmissionColor = attributes.diffuseColor;

        // Normal mapping
        if (valid(mat->map_Bump))
        {
            const vec3f n = get3f(mat->map_Bump, dg.st, make_vec3f(0.f)) - 0.5f;
            attributes.normal.x -= n.x;
            attributes.normal.y -= n.y;
        }

        // Reflection index
        attributes.reflection = mat->reflection;
        if (valid(mat->map_reflection))
        {
            const vec3f value = get3f(mat->map_reflection, dg.st);
            attributes.reflection *= (value.x + value.y + value.z) / 3.f;
        }

        // Cast simulation data
        attributes.castSimulationData = mat->castSimulationData;

        // Shading mode
        attributes.shadingMode = mat->shadingMode;
    }

    // Update reflected shading normal
    attributes.reflectedNormal =
        ray.dir - 2.f * dot(ray.dir, attributes.normal) * attributes.normal;
}

/**
  The purpose of the processSimulationContribution function is to retrieve the
  value of the simulation from a secondary model attached to the OSPRay
  scene.The geometry is defined by the primary model, but the shading is
  computed according to a second, non-visible model that contains the simulation
  values. In the case of circuit rendering for examples, neurons are meshes, and
  compartment reports are cones and cylinders that are inside the neuron. When
  an intersection is found at the surface of a mesh, a ray defined by the
  intersection point and the opposite normal to the surface is launched to hit
  the secondary model. When the secondary model is intersected, the simulation
  value is retrieved and used to shade the surface of the mesh.
*/
inline void processSimulationContribution(varying ScreenSample& sample,
                                          ShadingAttributes& attributes,
                                          const int materialID)
{
    if (!attributes.castSimulationData)
        return;

    if (!attributes.self->simulationModel)
    {
        const vec4f simulationColor =
            getSimulationValue(&attributes.self->super, attributes.dg);
        attributes.simulationColor = make_vec3f(simulationColor);
        attributes.simulationIntensity = simulationColor.w;
        return;
    }

    // Get simulation color from simulation model
    Ray colorRay;
    colorRay.org = attributes.origin;
    colorRay.dir =
        getRandomVector(attributes.self->super.super.super.fb->size.x, sample,
                        attributes.normal * -1.f,
                        attributes.self->randomNumber);
    colorRay.t0 = 0.f;
    colorRay.time = inf;
    colorRay.t = attributes.self->detectionDistance;
    colorRay.primID = -1;
    colorRay.geomID = -1;
    colorRay.instID = -1;

    // Trace ray to hit the simulation model
    traceRay(attributes.self->simulationModel, colorRay);

    if (colorRay.geomID < 0)
        return;

    // Intersection found. Simulation ray hit a primitive
    varying DifferentialGeometry colorDg;
    postIntersect(attributes.self->simulationModel, colorDg, colorRay,
                  DG_MATERIALID | DG_TEXCOORD);
    if (materialID == colorDg.materialID &&
        attributes.self->super.simulationData)
    {
        // The mesh and it's corresponding representation in the simulation
        // model must use the same material ID. This is to make sure that
        // one neuron is not shaded with the simulation value of another
        // neuron.
        const vec4f simulationColor =
            getSimulationValue(&attributes.self->super, &colorDg);
        attributes.simulationColor = make_vec3f(simulationColor);
        attributes.simulationIntensity = simulationColor.w;
    }
}

inline void processBackgroundColor(varying ScreenSample& sample,
                                   const varying Ray& ray,
                                   ShadingAttributes& attributes)
{
    // No intersection. Return volume contribution + skybox color
    const vec4f bgColor =
        skyboxMapping((Renderer*)attributes.self, ray,
                      attributes.self->super.super.bgMaterial);

    attributes.shadingContribution = make_vec3f(bgColor);
}

inline void computeIndirectShading(DifferentialGeometry& dg,
                                   varying ScreenSample& sample,
                                   ShadingAttributes& attributes)

{
    if (attributes.self->ambientOcclusionStrength <
        attributes.self->samplingThreshold)
        return;

    indirectShading(attributes.self, sample, attributes.origin,
                    attributes.normal, dg, attributes.indirectColor,
                    attributes.indirectIntensity);
}

inline void processShadows(DifferentialGeometry& dg,
                           varying ScreenSample& sample, const varying Ray& ray,
                           ShadingAttributes& attributes)
{
    for (uniform int i = 0; i < attributes.self->super.super.numLights; ++i)
    {
        const uniform Light* uniform light =
            attributes.self->super.super.lights[i];
        const vec2f samplingLocation = make_vec2f(0.5f);
        const varying Light_SampleRes lightSample =
            light->sample(light, dg, samplingLocation);
        const float radiance = reduce_max(lightSample.weight);
        const vec3f lightDirection = lightSample.dir;

        if (radiance <= 0.f)
            continue;

        const bool shadowsEnabled = attributes.lightEmissionIntensity <
                                    attributes.self->samplingThreshold;
        attributes.shadowIntensity +=
            shadowsEnabled ? shadedLightIntensity(sample, ray, attributes,
                                                  lightDirection, dg)
                           : 0.f;
    }

    if (attributes.shadowIntensity > 0.f)
        // Remove specular color if surface is in the shades
        attributes.specularColor = make_vec3f(0.f);
}

inline void processLightShading(DifferentialGeometry& dg,
                                varying ScreenSample& sample,
                                const varying Ray& ray,
                                ShadingAttributes& attributes)
{
    // Shading for all lights defined in the scene
    attributes.shadingContribution = make_vec3f(0.f);

    for (uniform int i = 0; i < attributes.self->super.super.numLights; ++i)
    {
        const uniform Light* uniform light =
            attributes.self->super.super.lights[i];
        const vec2f samplingLocation = make_vec2f(0.5f);
        const varying Light_SampleRes lightSample =
            light->sample(light, dg, samplingLocation);
        const float radiance = reduce_max(lightSample.weight);
        const vec3f lightDirection = lightSample.dir;

        if (radiance <= 0.f)
            continue;

        float cosNL = 1.f;

        if (attributes.shadingMode == diffuse)
        {
            // Phong + Blinn shading
            cosNL = max(0.f, dot(lightDirection, attributes.normal));

            // Specular color
            const float cosLR = dot(lightDirection, attributes.reflectedNormal);
            attributes.specularColor = attributes.specularColor *
                                       powf(cosLR, attributes.specularExponent);
        }
        else
            attributes.specularColor = make_vec3f(0.f);

        attributes.diffuseColor = cosNL * attributes.diffuseColor;
        attributes.simulationColor = cosNL * attributes.simulationColor;

        // Process light contribution
        attributes.shadingContribution =
            attributes.shadingContribution + radiance * attributes.diffuseColor;
    }
}

inline void processElectronShading(varying ScreenSample& sample,
                                   ShadingAttributes& attributes)
{
    const vec3f viewer = normalize(sample.ray.org - attributes.origin);
    const float el = max(0.f, dot(viewer, attributes.normal));
    const float cosNL = 1.f - el;
    attributes.shadingContribution = cosNL * attributes.diffuseColor;
    attributes.simulationColor = cosNL * attributes.simulationColor;
    attributes.specularColor = make_vec3f(0.f);
}

inline void processVolumeContribution(varying ScreenSample& sample,
                                      const varying Ray& ray,
                                      ShadingAttributes& attributes)
{
    for (uniform int32 i = 0;
         i < attributes.self->super.super.super.model->volumeCount; i++)
    {
        Volume* uniform volume =
            attributes.self->super.super.super.model->volumes[i];

        const vec4f volumetricValue =
            getVolumeContribution(volume, attributes.self, ray, sample);
        attributes.volumeColor =
            attributes.volumeColor + make_vec3f(volumetricValue);
        attributes.volumeIntensity += volumetricValue.w;
    }
}

inline void processFinalContribution(ShadingAttributes& attributes)
{
    // Indirect light contribution
    const vec3f indirectContribution = attributes.opacity *
                                       attributes.indirectColor *
                                       attributes.indirectIntensity;

    // Specular contribution
    vec3f totalContributions =
        attributes.shadingContribution + attributes.specularColor;

    // Surface shading + volume final contribution
    totalContributions =
        attributes.simulationColor * attributes.simulationIntensity +
        totalContributions * (1.f - attributes.simulationIntensity) +
        indirectContribution;

    // Shadow contribution
    totalContributions =
        totalContributions * make_vec3f(1.f - attributes.shadowIntensity);

    // Light emission contribution
    totalContributions =
        make_vec3f(totalContributions) +
        attributes.lightEmissionIntensity * attributes.lightEmissionColor;

    // Volume contribution
    if (attributes.volumeIntensity > 0.f)
    {
        totalContributions =
            attributes.volumeColor * attributes.volumeIntensity +
            totalContributions * (1.f - attributes.volumeIntensity);
        attributes.opacity += attributes.volumeIntensity;
    }

    // Store final contribution
    attributes.finalContribution =
        clamp(totalContributions, make_vec3f(0.f), make_vec3f(1.f));
}

inline vec3f AdvancedSimulationRenderer_shadeRay(
    const uniform AdvancedSimulationRenderer* uniform self,
    varying ScreenSample& sample)
{
    Ray ray = sample.ray;
    vec3f color = make_vec3f(0.f);

    unsigned int depth = 0;
    float oldlocalRefraction = 1.f;
    bool moreRebounds = true;
    float pathOpacity = 1.f;
    float totalOpacity = 0.f;

    sample.z = inf;

    while (moreRebounds && depth < NB_MAX_REBOUNDS && pathOpacity > 0.f)
    {
        // Shading attributes store all color contributions for the current
        // ray
        ShadingAttributes attributes;
        initializeShadingAttributes(self, attributes);

        // Trace ray
        traceRay(self->super.super.super.model, ray);

        float epsilon = 0.f;

        if (ray.geomID < 0)
        {
            // Background contribution
            processBackgroundColor(sample, ray, attributes);

            // Volume contribution
            processVolumeContribution(sample, ray, attributes);

            // No Geometry intersection. No need to iterate more
            moreRebounds = false;

            // Alpha
            totalOpacity += attributes.volumeIntensity;
        }
        else
        {
            // Get intersection information
            DifferentialGeometry dg;
            postIntersect(self->super.super.super.model, dg, ray,
                          DG_NG | DG_NS | DG_NORMALIZE | DG_FACEFORWARD |
                              DG_TANGENTS | DG_MATERIALID | DG_COLOR |
                              DG_TEXCOORD);

            // Initialize geometry shading attributes
            setGeometryShadingAttributes(self, dg, sample, ray, attributes);

            // Compute simulation contribution
            processSimulationContribution(sample, attributes, dg.materialID);

            // Z-Depth
            if (depth == 0)
                sample.z = ray.t;

            if (attributes.opacity > self->samplingThreshold && moreRebounds)
            {
                // Compute indirect lighting contribution
                computeIndirectShading(dg, sample, attributes);

                // Compute surface shading
                if (attributes.shadingMode == electron)
                    processElectronShading(sample, attributes);
                else
                    processLightShading(dg, sample, ray, attributes);

                // Compute shadows
                if (attributes.self->shadows > 0.f)
                    processShadows(dg, sample, ray, attributes);

                // Compute volume contribution
                processVolumeContribution(sample, ray, attributes);
            }
            else
                // Fully transparent object. Ignore intersection
                ray.t0 = ray.t + 1.f;

            // Alpha
            totalOpacity += attributes.opacity;

            epsilon = dg.epsilon;
        }

        // Combine all contributions
        processFinalContribution(attributes);

        // Pixel Color
        color = attributes.finalContribution * pathOpacity +
                color * (1.f - pathOpacity);

        // Prepare ray for next iteration
        bool doRefraction = (attributes.opacity < 1.f);
        bool doReflection = (attributes.reflection > 0.f);
        if (doRefraction && doReflection)
        {
            // Switch between refraction and reflection
            const bool reflect = sample.sampleID.z % 4 == 1;
            doRefraction = !reflect;
            doReflection = reflect;
        }

        ray.org = attributes.origin;
        ray.t0 = epsilon;
        ray.t = inf;
        ray.primID = -1;
        ray.geomID = -1;
        ray.instID = -1;

        if (doRefraction)
        {
            // Refraction
            ray.dir =
                refractedVector(ray.dir, attributes.normal, oldlocalRefraction,
                                attributes.refraction);
            pathOpacity *= 1.f - attributes.opacity;
            oldlocalRefraction = attributes.refraction;
        }
        else if (doReflection)
        {
            // Reflection
            ray.dir = attributes.reflectedNormal;
            pathOpacity *= attributes.reflection;
        }
        else
            moreRebounds = false;

        ++depth;
    }

    sample.alpha = min(1.f, totalOpacity);
    return color;
}

void AdvancedSimulationRenderer_renderSample(uniform Renderer* uniform _self,
                                             void* uniform perFrameData,
                                             varying ScreenSample& sample)
{
    uniform AdvancedSimulationRenderer* uniform self =
        (uniform AdvancedSimulationRenderer * uniform) _self;
    sample.ray.time = infinity;
    sample.rgb = AdvancedSimulationRenderer_shadeRay(self, sample);
}

// Exports (called from C++)
export void* uniform AdvancedSimulationRenderer_create(void* uniform cppE)
{
    uniform AdvancedSimulationRenderer* uniform self =
        uniform new uniform AdvancedSimulationRenderer;
    Renderer_Constructor(&self->super.super.super, cppE);
    self->super.super.super.renderSample =
        AdvancedSimulationRenderer_renderSample;
    return self;
}

export void AdvancedSimulationRenderer_set(
    void* uniform _self, void* uniform simulationModel,
    void* uniform bgMaterial, const uniform float& shadows,
    const uniform float& softShadows,
    const uniform float& ambientOcclusionStrength,
    const uniform float& ambientOcclusionDistance,
    const uniform int& randomNumber, const uniform float& timestamp,
    const uniform int& spp, void** uniform lights,
    const uniform int32 numLights, const uniform int32& volumeSamplesPerRay,
    uniform float* uniform simulationData,
    const uniform uint64& simulationDataSize, uniform vec4f* uniform colormap,
    uniform vec3f* uniform emissionIntensitiesMap,
    const uniform int32 colorMapSize, const uniform float& colorMapMinValue,
    const uniform float& colorMapRange, const uniform float& samplingThreshold,
    const uniform float& detectionDistance,
    const uniform float& volumeSpecularExponent,
    const uniform float& volumeAlphaCorrection)
{
    uniform AdvancedSimulationRenderer* uniform self =
        (uniform AdvancedSimulationRenderer * uniform) _self;

    self->super.super.super.spp = spp;
    self->super.super.bgMaterial =
        (uniform ExtendedOBJMaterial * uniform) bgMaterial;
    self->super.super.lights = (const uniform Light* uniform* uniform)lights;
    self->super.super.numLights = numLights;
    self->super.super.timestamp = timestamp;

    self->shadows = shadows;
    self->softShadows = softShadows;
    self->ambientOcclusionStrength = ambientOcclusionStrength;
    self->ambientOcclusionDistance = ambientOcclusionDistance;
    self->randomNumber = randomNumber;

    self->volumeSamplesPerRay = volumeSamplesPerRay;

    self->super.colorMap = (uniform vec4f * uniform) colormap;
    self->super.emissionIntensitiesMap =
        (uniform vec3f * uniform) emissionIntensitiesMap;
    self->super.colorMapSize = colorMapSize;
    self->super.colorMapMinValue = colorMapMinValue;
    self->super.colorMapRange = colorMapRange;

    self->super.simulationData = (uniform float* uniform)simulationData;
    self->super.simulationDataSize = simulationDataSize;

    self->simulationModel = (uniform Model * uniform) simulationModel;
    self->detectionDistance = detectionDistance;

    self->samplingThreshold = samplingThreshold;

    self->volumeSpecularExponent = volumeSpecularExponent;
    self->volumeAlphaCorrection = volumeAlphaCorrection;
}
