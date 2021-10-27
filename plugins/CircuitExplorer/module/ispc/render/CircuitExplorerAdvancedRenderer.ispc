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

struct CircuitExplorerAdvancedRenderer
{
    CircuitExplorerSimulationRenderer super;

    // Shading attributes
    uint32 randomNumber;
    float shadows;
    float softShadows;
    uint32 softShadowsSamples;
    float giStrength;
    float giDistance;
    uint32 giSamples;
    float epsilonFactor;

    // Volumes
    uint32 volumeSamplesPerRay;
    float samplingThreshold;
    float volumeSpecularExponent;
    float volumeAlphaCorrection;

    // Clip planes
    const uniform vec4f* clipPlanes;
    uint32 numClipPlanes;
};

struct ShadingAttributes
{
    const uniform CircuitExplorerAdvancedRenderer* uniform self;
    varying DifferentialGeometry* dg;
    vec4f bgColor;
    vec3f origin;
    vec3f normal;
    vec3f reflectedNormal;
    float refraction;
    float reflection;
    float opacity;
    float glossiness;
    vec3f diffuseColor;
    vec3f specularColor;
    float specularExponent;
    vec3f lightEmissionColor;
    float lightEmissionIntensity;
    float shadowIntensity;
    bool castSimulationData;
    MaterialClippingMode clippingMode;
    MaterialShadingMode shadingMode;
    float userParameter;
    vec3f simulationColor;
    float simulationIntensity;
    vec3f indirectColor;
    float indirectIntensity;
    vec3f volumeColor;
    float volumeIntensity;
    vec3f shadingContribution;
    vec3f finalContribution;
};

bool isClipped(const uniform CircuitExplorerAdvancedRenderer* uniform self,
               const varying vec3f& position,
               const MaterialClippingMode clippingMode)
{
    if (clippingMode == no_clipping)
        return false;

    bool visible = true;
    for (uint32 i = 0; i < self->numClipPlanes && visible; ++i)
    {
        if (clippingMode == plane)
        {
            const vec3f normal = {self->clipPlanes[i].x, self->clipPlanes[i].y,
                                  self->clipPlanes[i].z};
            const float d = self->clipPlanes[i].w;
            const float distance = dot(normal, position) + d;
            visible &= (distance > 0.f);
        }
        else if (clippingMode == sphere)
        {
            const vec3f center = {self->clipPlanes[i].x, self->clipPlanes[i].y,
                                  self->clipPlanes[i].z};
            const float radius = self->clipPlanes[i].w;
            visible &= length(center - position) < radius;
        }
    }
    return !visible;
}

inline bool launchRandomRay(
    const uniform CircuitExplorerAdvancedRenderer* uniform self,
    varying ScreenSample& sample, const varying vec3f& intersection,
    const varying vec3f& normal, DifferentialGeometry& geometry,
    varying vec3f& backgroundColor, varying float& distanceToIntersection,
    varying vec3f& randomDirection, const uint32 iteration)
{
    randomDirection =
        getRandomVector(self->super.super.useHardwareRandomizer,
                        self->super.super.super.fb->size.x, sample, normal,
                        iteration + self->randomNumber);
    backgroundColor = make_vec3f(0.f);

    if (dot(randomDirection, normal) < 0.f)
        // Invert direction of random ray direction is opposite to surface
        // normal
        randomDirection = neg(randomDirection);

    Ray randomRay;
    randomRay.org = intersection;
    randomRay.dir = randomDirection;
    randomRay.t0 = max(0.f, geometry.epsilon);
    randomRay.t = self->giDistance;
    randomRay.time = inf;
    randomRay.primID = -1;
    randomRay.geomID = -1;
    randomRay.instID = -1;

    traceRay(self->super.super.super.model, randomRay);

    if (randomRay.geomID < 0)
    {
        // No intersection
        backgroundColor = make_vec3f(
            skyboxMapping((Renderer*)self, randomRay,
                          (DefaultMaterial*)self->super.super.bgMaterial));
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
    const uniform CircuitExplorerAdvancedRenderer* uniform self,
    varying ScreenSample& sample, const varying vec3f& intersection,
    const varying vec3f& normal, varying vec3f& indirectShadingColor,
    varying float& indirectShadingPower)
{
    if (self->giSamples == 0)
        return;

    vec3f backgroundColor = make_vec3f(0.f);
    float distanceToIntersection = inf;

    float shadingPower = 0.f;
    vec3f shadingColor = make_vec3f(0.f);

    for (uint32 i = 0; i < self->giSamples; ++i)
    {
        // Launch a random ray
        vec3f randomDirection;
        DifferentialGeometry geometry;
        if (launchRandomRay(self, sample, intersection, normal, geometry,
                            backgroundColor, distanceToIntersection,
                            randomDirection, i))
        {
            // Determine material of intersected geometry
            const uniform Material* material = geometry.material;
            const uniform CircuitExplorerMaterial* objMaterial =
                (const uniform CircuitExplorerMaterial*)material;
            if (objMaterial)
                foreach_unique(mat in objMaterial)
                {
                    if (mat->a > 0.f)
                    {
                        if (valid(mat->map_Kd))
                            shadingColor =
                                shadingColor + get3f(mat->map_Kd, geometry);
                        else
                            shadingColor = shadingColor + mat->Kd;

                        // Emission
                        if (valid(mat->map_a))
                        {
                            vec3f Ka = get3f(mat->map_a, geometry);
                            shadingPower = shadingPower + reduce_max(Ka);
                        }
                        else
                            shadingPower = shadingPower + mat->a;
                    }
                }
        }
        else
        {
            shadingColor = shadingColor + backgroundColor;
            shadingPower = shadingPower + DEFAULT_SKY_POWER;
        }
    }

    indirectShadingColor =
        indirectShadingColor + shadingColor / (float)(self->giSamples);
    indirectShadingPower = self->giStrength * indirectShadingPower +
                           shadingPower / (float)(self->giSamples);
}

inline float getVolumeShadowContribution(
    Volume* uniform volume,
    const uniform CircuitExplorerAdvancedRenderer* uniform self,
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
        if (isClipped(self, point, plane))
            continue;
        const float sample = volume->sample(volume, point);

        // Look up the opacity associated with the volume sample.
        shadowIntensity += volume->transferFunction->getOpacityForValue(
            volume->transferFunction, sample);
    }
    return shadowIntensity;
}

inline float getVolumeShadowContributions(
    Volume* uniform volume, const uniform uint32 uniform lightIndex,
    const uniform CircuitExplorerAdvancedRenderer* uniform self,
    const varying Ray& ray, varying ScreenSample& sample, const vec3f& point,
    const float epsilon)
{
    float shadowIntensity = 0.f;
    const uniform Light* uniform light = self->super.super.lights[lightIndex];
    const varying vec2f s = make_vec2f(0.5f);
    DifferentialGeometry dg;
    dg.P = point;
    const varying Light_SampleRes lightSample = light->sample(light, dg, s);

    Ray lightRay = ray;
    lightRay.t = inf;
    lightRay.t0 = max(0.f, epsilon);
    lightRay.org = point;
    lightRay.geomID = -1;

    if (self->softShadows > 0.f)
        lightRay.dir = normalize(
            lightSample.dir +
            self->softShadows *
                getRandomVector(self->super.super.useHardwareRandomizer,
                                self->super.super.super.fb->size.x, sample,
                                lightSample.dir, self->randomNumber));
    else
        lightRay.dir = lightSample.dir;

    // Intersection with Geometry
    traceRay(self->super.super.super.model, lightRay);
    if (lightRay.geomID != -1)
    {
        shadowIntensity += 1.f;
        return shadowIntensity * self->shadows;
    }

    // Intersection with volume
    shadowIntensity +=
        getVolumeShadowContribution(volume, self, lightRay, sample);
    return shadowIntensity * self->shadows;
}

inline vec4f getVolumeContribution(
    Volume* uniform volume,
    const uniform CircuitExplorerAdvancedRenderer* uniform self,
    varying Ray& ray, varying ScreenSample& sample, float& firstIntersection)
{
    // Find volume intersections
    float t0, t1;
    intersectBox(ray, volume->boundingBox, t0, t1);
    t1 = min(ray.t, t1);

    vec4f pathColor = make_vec4f(0.f);
    float epsilon = volume->samplingStep;
    float shadowIntensity = 0.f;

    // Ray marching
    uint32 shadingOccurence = 0;
    for (float t = t0 + epsilon /** (sample.sampleID.z % 100)*/;
         t < t1 && pathColor.w < 1.f; t += epsilon)
    {
        const vec3f point = ray.org + t * ray.dir;
        const float volumeSample = volume->sample(volume, point);

        // Look up the opacity associated with the volume sample
        const float sampleOpacity =
            volume->transferFunction->getOpacityForValue(
                volume->transferFunction, volumeSample);

        if (sampleOpacity <= self->samplingThreshold)
            // Continue walking for as long as voxel opacity is below
            // sampling threshold
            continue;

        if (shadingOccurence == 0)
        {
            firstIntersection = t;
            ray.t = t;
            // Introduce a bit of randomness to smooth the shading
            t += getRandomValue(self->super.super.useHardwareRandomizer, sample,
                                self->randomNumber) *
                 ((t1 - t0) * 0.01f);
        }

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
            (firstShadingOccurence && self->giStrength > 0.f);

        const bool shadowsEnabled =
            (firstShadingOccurence && self->shadows > 0.f);

        if (shadingEnabled || aoEnabled)
        {
            gradient = safe_normalize(volume->computeGradient(volume, point));
            // Setup differential geometry for the volume sample point.
            dg.P = point;
            dg.epsilon = self->epsilonFactor * epsilon;
            dg.Ns = gradient;
        }

        if (shadingEnabled)
        {
            // Compute light contributions
            vec3f shadedColor = make_vec3f(0.f);
            for (uniform uint32 i = 0; i < self->super.super.numLights; ++i)
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

                // Shadow
                if (shadowsEnabled)
                    // Compute shadow contribution
                    shadowIntensity =
                        getVolumeShadowContributions(volume, i, self, ray,
                                                     sample, point, epsilon);
                shadedColor = shadedColor * (1.f - shadowIntensity);
            }
            volumeSampleColor = shadedColor;
        }

        if (aoEnabled)
        {
            // Compute ambient occlusion contribution
            vec3f indirectColor;
            float indirectIntensity;
            indirectShading(self, sample, point, gradient, indirectColor,
                            indirectIntensity);
            volumeSampleColor = volumeSampleColor + sampleOpacity *
                                                        indirectColor *
                                                        indirectIntensity;
        }

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
    float intensity = 0.f;

    const uint32 nbSamples = (attributes.self->softShadows > 0.f
                                  ? attributes.self->softShadowsSamples
                                  : 1);

    for (uint32 s = 0; s < nbSamples; ++s)
    {
        float shadowIntensity = 0.f;

        vec3f ld = lightDirection;
        if (attributes.self->softShadows > 0.f)
            // Slightly alter light direction for Soft shadows
            ld = normalize(
                ld + attributes.self->softShadows *
                         getRandomVector(
                             attributes.self->super.super.useHardwareRandomizer,
                             attributes.self->super.super.super.fb->size.x,
                             sample, attributes.normal,
                             s + attributes.self->randomNumber));

        if (dot(ld, lightDirection) < 0.f)
            ld = neg(ld);

        Ray shadowRay;
        shadowRay.org = dg.P;
        shadowRay.dir = ld;
        shadowRay.t0 = max(0.f, attributes.self->epsilonFactor * dg.epsilon);
        shadowRay.t = attributes.self->giDistance;
        shadowRay.time = inf;
        shadowRay.geomID = -1;
        shadowRay.primID = -1;
        shadowRay.instID = -1;

        while (shadowIntensity < 1.f)
        {
            traceRay(attributes.self->super.super.super.model, shadowRay);

            if (shadowRay.geomID == -1)
                break;

            DifferentialGeometry shadowingGeometry;
            postIntersect(attributes.self->super.super.super.model,
                          shadowingGeometry, shadowRay,
                          DG_MATERIALID | DG_TEXCOORD | DG_COLOR);

            if (!isClipped(attributes.self, shadowingGeometry.P,
                           attributes.clippingMode))
            {
                uniform CircuitExplorerMaterial* objMaterial =
                    (uniform CircuitExplorerMaterial*)
                        shadowingGeometry.material;

                float opacity = 1.f;

                if (objMaterial == 0)
                    opacity = 1.f - shadowingGeometry.color.w;
                else
                    foreach_unique(mat in objMaterial)
                    {
                        opacity = mat->d;
                        if (valid(mat->map_d))
                        {
                            vec4f d_map = get4f(mat->map_d, shadowingGeometry);
                            opacity = d_map.x;
                        }
                        else if (valid(mat->map_Kd))
                        {
                            vec4f localDiffuseColor_map =
                                get4f(mat->map_Kd, shadowingGeometry);
                            opacity = 1.f - localDiffuseColor_map.w;
                        }
                    }

                shadowIntensity += opacity;
            }

            shadowRay.t0 = max(0.f, shadowRay.t + shadowingGeometry.epsilon);
            shadowRay.t = inf;
            shadowRay.primID = -1;
            shadowRay.geomID = -1;
            shadowRay.instID = -1;
        }
        intensity += shadowIntensity;

        // Volume shadows
        shadowRay.org = dg.P;
        shadowRay.t0 = max(0.f, dg.epsilon);

        for (uniform uint32 i = 0;
             shadowIntensity < 1.f &&
             i < attributes.self->super.super.super.model->volumeCount;
             ++i)
        {
            Volume* uniform volume =
                attributes.self->super.super.super.model->volumes[i];

            shadowIntensity +=
                getVolumeShadowContribution(volume, attributes.self, shadowRay,
                                            sample) *
                attributes.self->shadows;
        }
    }
    intensity /= (float)nbSamples;

    return clamp(intensity * attributes.self->shadows, 0.f, 1.f);
}

inline void initializeShadingAttributes(
    const uniform CircuitExplorerAdvancedRenderer* uniform self,
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

    // Clipping planes
    attributes.clippingMode = no_clipping;

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
    attributes.glossiness = 0.f;
}

inline void setGeometryShadingAttributes(
    const uniform CircuitExplorerAdvancedRenderer* uniform self,
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
    const uniform CircuitExplorerMaterial* objMaterial =
        (const uniform CircuitExplorerMaterial*)material;
    if (!objMaterial)
    {
        // Diffuse color
        attributes.diffuseColor = make_vec3f(dg.color);

        // if no material is defined, localOpacity is stored in the
        // alpha
        // component of the RGBA color
        attributes.opacity = dg.color.w;
        return;
    }

    foreach_unique(mat in objMaterial)
    {
        // Glossiness
        if (mat->glossiness < 1.f)
        {
            attributes.glossiness = mat->glossiness;
            const vec3f randomNormal =
                (1.f - mat->glossiness) *
                getRandomVector(self->super.super.useHardwareRandomizer,
                                self->super.super.super.fb->size.x, sample,
                                attributes.normal, self->randomNumber);
            attributes.normal = normalize(attributes.normal + randomNormal);
        }

        // Refraction index
        attributes.refraction = mat->refraction;

        // Opacity
        if (valid(mat->map_d))
            attributes.opacity = mat->d * get1f(mat->map_d, dg);
        else
            attributes.opacity = mat->d;

        // Diffuse color
        attributes.diffuseColor = mat->Kd * make_vec3f(dg.color);
        if (valid(mat->map_Kd))
        {
            const vec3f diffuseColorFromMap = get3f(mat->map_Kd, dg);
            attributes.diffuseColor =
                attributes.diffuseColor * make_vec3f(diffuseColorFromMap);
            //            attributes.opacity *= diffuseColorFromMap.w;
        }

        // Specular color
        if (valid(mat->map_Ks))
            attributes.specularColor =
                max(attributes.specularColor, mat->Ks * get3f(mat->map_Ks, dg));
        else
            attributes.specularColor = max(attributes.specularColor, mat->Ks);

        // Specular exponent
        if (valid(mat->map_Ns))
            attributes.specularExponent = max(attributes.specularExponent,
                                              mat->Ns * get1f(mat->map_Ns, dg));
        else
            attributes.specularExponent =
                max(attributes.specularExponent, mat->Ns);

        // Light emission intensity
        attributes.lightEmissionIntensity += mat->a;
        attributes.lightEmissionColor = attributes.diffuseColor;

        // Normal mapping
        if (valid(mat->map_Bump))
        {
            const vec3f n = get3f(mat->map_Bump, dg, make_vec3f(0.f)) - 0.5f;
            attributes.normal.x -= n.x;
            attributes.normal.y -= n.y;
        }

        // Reflection index
        attributes.reflection = mat->reflection;
        if (valid(mat->map_reflection))
        {
            const vec3f value = get3f(mat->map_reflection, dg);
            attributes.reflection *= (value.x + value.y + value.z) / 3.f;
        }

        // Cast simulation data
        attributes.castSimulationData = mat->castSimulationData;

        // Shading mode
        attributes.shadingMode = mat->shadingMode;

        // Clipping planes
        attributes.clippingMode = mat->clippingMode;

        // User parameter
        attributes.userParameter = mat->userParameter;
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
                                          const uint32 materialID,
                                          const varying uint32 primID)
{
    const uniform Geometry* geometry = attributes.dg->geometry;
    const uniform OSPRayISPCModel* model = (const uniform OSPRayISPCModel*)geometry->model;

    if(model->simEnabled != 1)
        return;

    if (!attributes.castSimulationData)
        return;

    if (!attributes.self->super.simulationData)
        return;

    if (!attributes.self->super.secondaryModel)
    {
        const vec4f simulationColor =
            getSimulationValue(&attributes.self->super, attributes.dg, primID);
        attributes.simulationColor = make_vec3f(simulationColor);
        attributes.simulationIntensity = simulationColor.w;
        return;
    }

    // Get simulation color from simulation model
    Ray colorRay;
    colorRay.org = attributes.origin;
    colorRay.dir =
        getRandomVector(attributes.self->super.super.useHardwareRandomizer,
                        attributes.self->super.super.super.fb->size.x, sample,
                        neg(attributes.normal), attributes.self->randomNumber);
    colorRay.t0 = 0.f;
    colorRay.time = inf;
    colorRay.t = attributes.self->super.maxDistanceToSecondaryModel;
    colorRay.primID = -1;
    colorRay.geomID = -1;
    colorRay.instID = -1;

    // Trace ray to hit the simulation model
    traceRay(attributes.self->super.secondaryModel, colorRay);

    if (colorRay.geomID < 0)
        return;

    // Intersection found. Simulation ray hit a primitive
    varying DifferentialGeometry colorDg;
    postIntersect(attributes.self->super.secondaryModel, colorDg, colorRay,
                  DG_MATERIALID | DG_TEXCOORD);
    if (attributes.self->super.simulationData)
    {
        // The mesh and it's corresponding representation in the simulation
        // model must use the same material ID. This is to make sure that one
        // neuron is not shaded with the simulation value of another neuron.
        const vec4f simulationColor =
            getSimulationValue(&attributes.self->super, &colorDg,
                               colorRay.primID);
        attributes.simulationColor = make_vec3f(simulationColor);
        attributes.simulationIntensity = simulationColor.w;
    }
}

inline void processBackgroundColor(varying ScreenSample& sample,
                                   const varying Ray& ray,
                                   ShadingAttributes& attributes)
{
    // No intersection. Return volume contribution + skybox color
    attributes.bgColor = skyboxMapping(
        (Renderer*)attributes.self, ray,
        (DefaultMaterial*)attributes.self->super.super.bgMaterial);
    attributes.shadingContribution = make_vec3f(attributes.bgColor);
}

inline void computeIndirectShading(DifferentialGeometry& dg,
                                   varying ScreenSample& sample,
                                   ShadingAttributes& attributes)

{
    if (attributes.self->giStrength < attributes.self->samplingThreshold)
        return;

    indirectShading(attributes.self, sample, attributes.origin,
                    attributes.normal, attributes.indirectColor,
                    attributes.indirectIntensity);
}

inline void processShadows(DifferentialGeometry& dg,
                           varying ScreenSample& sample, const varying Ray& ray,
                           ShadingAttributes& attributes)
{
    for (uniform uint32 i = 0; i < attributes.self->super.super.numLights; ++i)
    {
        const uniform Light* uniform light =
            attributes.self->super.super.lights[i];
        const vec2f samplingLocation = make_vec2f(0.5f);
        const varying Light_SampleRes lightSample =
            light->sample(light, dg, samplingLocation);
        const vec3f lightColor = lightSample.weight;
        const float radiance = reduce_max(lightColor);
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

    vec3f accDiffuse = make_vec3f(0.f);
    vec3f accSimulation = make_vec3f(0.f);
    vec3f accSpecular = make_vec3f(0.f);

    for (uniform uint32 i = 0; i < attributes.self->super.super.numLights; ++i)
    {
        const uniform Light* uniform light =
            attributes.self->super.super.lights[i];
        const vec2f samplingLocation = make_vec2f(0.5f);
        const varying Light_SampleRes lightSample =
            light->sample(light, dg, samplingLocation);
        const vec3f lightColor = lightSample.weight;
        const float radiance = reduce_max(lightColor);

        if (radiance <= 0.f)
            continue;

        const vec3f lightDirection = lightSample.dir;
        float cosNL = 1.f;

        if (attributes.shadingMode != none)
        {
            // Phong + Blinn shading
            if (attributes.shadingMode != perlin)
                cosNL = max(0.f, dot(lightDirection, attributes.normal));

            // Specular color
            const float cosLR = dot(lightDirection, attributes.reflectedNormal);
            accSpecular = accSpecular +
                          lightColor * powf(cosLR, attributes.specularExponent);
        }

        accDiffuse =
            accDiffuse + (lightColor * cosNL * attributes.diffuseColor);
        accSimulation =
            accSimulation + (lightColor * cosNL * attributes.simulationColor);

        if (attributes.shadingMode == diffuse_transparency)
            attributes.opacity *= sqrt(cosNL);
    }

    // Process light contribution
    attributes.specularColor = accSpecular;
    attributes.simulationColor = accSimulation;
    attributes.shadingContribution = accDiffuse;
}

inline void processElectronShading(varying ScreenSample& sample,
                                   ShadingAttributes& attributes)
{
    const vec3f viewer = normalize(sample.ray.org - attributes.origin);
    const float el = max(0.f, dot(viewer, attributes.normal));
    const float cosNL = 1.f - pow(el, attributes.userParameter);
    attributes.shadingContribution = cosNL * attributes.diffuseColor;
    attributes.simulationColor = cosNL * attributes.simulationColor;
    attributes.specularColor = make_vec3f(0.f);
}

inline void processElectronTransparencyShading(varying ScreenSample& sample,
                                               ShadingAttributes& attributes)
{
    const vec3f viewer = normalize(sample.ray.org - attributes.origin);
    const float el = 1.f - max(0.f, dot(viewer, attributes.normal));
    const float cosNL = 1.f - pow(el, attributes.userParameter);
    attributes.shadingContribution = cosNL * attributes.diffuseColor;
    attributes.simulationColor = cosNL * attributes.simulationColor;
    attributes.specularColor = make_vec3f(0.f);
    attributes.opacity *= cosNL;
}

inline void processCartoonShading(varying ScreenSample& sample,
                                  ShadingAttributes& attributes)
{
    const vec3f viewer = normalize(sample.ray.org - attributes.origin);
    float cosNL = max(0.f, dot(viewer, attributes.normal));
    const uint32 angleAsInt = cosNL * attributes.userParameter;
    cosNL = (float)(angleAsInt) / attributes.userParameter;

    attributes.shadingContribution = cosNL * attributes.diffuseColor;
    attributes.simulationColor = cosNL * attributes.simulationColor;
    attributes.specularColor = make_vec3f(0.f);
}

inline void processNoiseShading(DifferentialGeometry& dg,
                                varying ScreenSample& sample,
                                const varying Ray& ray,
                                ShadingAttributes& attributes)
{
    vec3f point = attributes.userParameter * dg.P;

    const float n1 = 0.25f + 0.75f * clamp(worleyNoise(point, 2.f), 0.f, 1.f);

    attributes.diffuseColor = (1.f - n1) * attributes.diffuseColor;
    attributes.simulationColor = (1.f - n1) * attributes.simulationColor;
    attributes.specularColor = (1.f - n1) * attributes.specularColor;

    attributes.normal.x += 0.5f * n1;
    attributes.normal.y += 0.5f * (0.5f - n1);
    attributes.normal.z += 0.5f * (0.25f - n1);
    attributes.normal = normalize(attributes.normal);

    attributes.reflectedNormal =
        ray.dir - 2.f * dot(ray.dir, attributes.normal) * attributes.normal;

    processLightShading(dg, sample, ray, attributes);
}

inline void processCheckerShading(DifferentialGeometry& dg,
                                  varying ScreenSample& sample,
                                  const varying Ray& ray,
                                  ShadingAttributes& attributes)
{
    const vec3i point =
        make_vec3i(attributes.userParameter * (dg.P + make_vec3f(1e2f)));
    const vec3i p = make_vec3i(point.x % 2, point.y % 2, point.z % 2);
    if ((p.x == p.y && p.z == 1) || (p.x != p.y && p.z == 0))
    {
        attributes.diffuseColor = make_vec3f(1.f) - attributes.diffuseColor;
        attributes.simulationColor =
            make_vec3f(1.f) - attributes.simulationColor;
        attributes.specularColor = make_vec3f(1.f) - attributes.specularColor;
    }
    processLightShading(dg, sample, ray, attributes);
}

inline void processVolumeContribution(varying ScreenSample& sample,
                                      varying Ray& ray,
                                      ShadingAttributes& attributes,
                                      float& firstIntersection)
{
    for (uniform int32 i = 0;
         i < attributes.self->super.super.super.model->volumeCount; i++)
    {
        Volume* uniform volume =
            attributes.self->super.super.super.model->volumes[i];

        const vec4f volumetricValue =
            getVolumeContribution(volume, attributes.self, ray, sample,
                                  firstIntersection);
        attributes.volumeColor =
            attributes.volumeColor + make_vec3f(volumetricValue);
        attributes.volumeIntensity += volumetricValue.w;
    }
}

inline void processFinalContribution(ShadingAttributes& attributes)
{
    // Surface shading + simulation + volume final contribution
    vec3f totalContributions =
        attributes.simulationColor * attributes.simulationIntensity +
        attributes.shadingContribution *
            (1.f - attributes.simulationIntensity) +
        attributes.specularColor;

    // Indirect light contribution
    totalContributions = totalContributions + attributes.indirectColor *
                                                  attributes.indirectIntensity;

    // Shadow contribution
    totalContributions =
        totalContributions * make_vec3f(1.f - attributes.shadowIntensity);

    // Light emission contribution
    totalContributions =
        make_vec3f(totalContributions) +
        attributes.lightEmissionIntensity * attributes.lightEmissionColor;

    // Volume contribution
    totalContributions =
        attributes.volumeColor * attributes.volumeIntensity +
        totalContributions * (1.f - attributes.volumeIntensity);
    attributes.opacity += attributes.volumeIntensity;

    // Store final contribution
    attributes.finalContribution =
        clamp(totalContributions, make_vec3f(0.f), make_vec3f(1.f));
}

inline vec3f CircuitExplorerAdvancedRenderer_shadeRay(
    const uniform CircuitExplorerAdvancedRenderer* uniform self,
    varying ScreenSample& sample)
{
    varying Ray ray = sample.ray;
    vec4f color = make_vec4f(0.f);
    vec3f bgColor = make_vec3f(0.f);

    uint32 depth = 0;
    float oldlocalRefraction = 1.f;
    bool moreRebounds = true;

    sample.z = inf;

    ShadingAttributes bgAttr;
    bgAttr.self = self;

    while (moreRebounds && depth < self->super.super.maxBounces &&
           color.w < 1.f)
    {
        float totalOpacity = 0.f;
        // Shading attributes store all color contributions for the
        // current ray
        ShadingAttributes attributes;
        initializeShadingAttributes(self, attributes);

        bool discardIntersection = false;
        float firstIntersection = inf;

        // Trace ray
        traceRay(self->super.super.super.model, ray);
        if (ray.geomID < 0)
        {
            // Background contribution
            processBackgroundColor(sample, ray, attributes);
            bgColor = make_vec3f(attributes.bgColor);

            // Volume contribution
            processVolumeContribution(sample, ray, attributes,
                                      firstIntersection);
            sample.z = min(sample.z, firstIntersection);

            // Alpha
            totalOpacity += attributes.volumeIntensity;

            // No Geometry intersection. No need to iterate anymore
            moreRebounds = false;
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

            if (isClipped(self, dg.P, attributes.clippingMode))
                // Geometry is clipped, discard intersection
                discardIntersection = true;
            else
            {
                // Geometry is not clipped, proceed with shading

                // Compute simulation contribution
                processSimulationContribution(sample, attributes, dg.materialID,
                                              ray.primID);

                if (attributes.opacity < self->samplingThreshold)
                    // Fully transparent object. Discard intersection
                    discardIntersection = true;
            }

            if (discardIntersection)
                // Continue along the same ray
                ray.t0 = max(0.f, ray.t + self->epsilonFactor * dg.epsilon);
            else
            {
                // Compute indirect lighting contribution
                computeIndirectShading(dg, sample, attributes);

                // Compute surface shading
                if (attributes.shadingMode == electron)
                    processElectronShading(sample, attributes);
                else if (attributes.shadingMode == cartoon)
                    processCartoonShading(sample, attributes);
                else if (attributes.shadingMode == electron_transparency)
                    processElectronTransparencyShading(sample, attributes);
                else if (attributes.shadingMode == perlin)
                    processNoiseShading(dg, sample, ray, attributes);
                else if (attributes.shadingMode == checker)
                    processCheckerShading(dg, sample, ray, attributes);
                else
                    processLightShading(dg, sample, ray, attributes);

                // Compute shadows
                if (attributes.self->shadows > 0.f)
                    processShadows(dg, sample, ray, attributes);

                // Compute volume contribution
                processVolumeContribution(sample, ray, attributes,
                                          firstIntersection);
                // Z-Depth
                if (depth == 0)
                    sample.z = min(ray.t, firstIntersection);

                // Alpha
                totalOpacity +=
                    (1.f - attributes.reflection) *
                    (attributes.opacity + attributes.volumeIntensity);

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

                if (doRefraction)
                {
                    // Refraction
                    ray.dir = refractedVector(ray.dir, attributes.normal,
                                              oldlocalRefraction,
                                              attributes.refraction);
                    oldlocalRefraction = attributes.refraction;
                }
                else if (doReflection)
                    // Reflection
                    ray.dir = attributes.reflectedNormal;
                else
                    moreRebounds = false;

                // Prepare next ray
                ray.org = dg.P + ray.dir * self->epsilonFactor * dg.epsilon;
                ray.t0 = max(0.f, self->epsilonFactor * dg.epsilon);
                ++depth;
            }

            ray.t = inf;
            ray.primID = -1;
            ray.geomID = -1;
            ray.instID = -1;
        }

        // Combine all contributions
        processFinalContribution(attributes);

        composite(make_vec4f(attributes.finalContribution, totalOpacity), color,
                  self->volumeAlphaCorrection);
    }

    // Fog attenuation
    const float distance = sample.z;
    const float fogAttenuation = distance > self->super.fogStart
                                     ? clamp((distance - self->super.fogStart) /
                                                 self->super.fogThickness,
                                             0.f, 1.f)
                                     : 0.f;
    const float fog = 1.f - color.w * (1.f - fogAttenuation);
    const vec3f finalColor = make_vec3f(color) * (1.f - fog) + fog * bgColor;

    // Final color
    sample.alpha = min(1.f, color.w);
    return make_vec3f(finalColor * self->super.super.exposure);
}

void CircuitExplorerAdvancedRenderer_renderSample(
    uniform Renderer* uniform _self, void* uniform perFrameData,
    varying ScreenSample& sample)
{
    uniform CircuitExplorerAdvancedRenderer* uniform self =
        (uniform CircuitExplorerAdvancedRenderer * uniform) _self;
    sample.ray.time = inf;
    sample.rgb = CircuitExplorerAdvancedRenderer_shadeRay(self, sample);
}

// Exports (called from C++)
export void* uniform CircuitExplorerAdvancedRenderer_create(void* uniform cppE)
{
    uniform CircuitExplorerAdvancedRenderer* uniform self =
        uniform new uniform CircuitExplorerAdvancedRenderer;
    Renderer_Constructor(&self->super.super.super, cppE);
    self->super.super.super.renderSample =
        CircuitExplorerAdvancedRenderer_renderSample;
    return self;
}

export void CircuitExplorerAdvancedRenderer_set(
    void* uniform _self, void* uniform secondaryModel,
    const uniform float maxDistanceToSecondaryModel, void* uniform bgMaterial,
    const uniform float shadows, const uniform float softShadows,
    const uniform uint32 softShadowsSamples, const uniform float giStrength,
    const uniform float giDistance, const uniform uint32 giSamples,
    const uniform uint32 randomNumber, const uniform float timestamp,
    const uniform uint32 spp, void** uniform lights,
    const uniform uint32 numLights, const uniform uint32 volumeSamplesPerRay,
    uniform float* uniform simulationData,
    const uniform uint64 simulationDataSize,
    const uniform float samplingThreshold,
    const uniform float volumeSpecularExponent,
    const uniform float volumeAlphaCorrection, const uniform float exposure,
    const uniform float fogThickness, const uniform float fogStart,
    const uniform vec4f clipPlanes[], const uniform uint32 numClipPlanes,
    const uniform uint32 maxBounces, const uniform float epsilonFactor,
    const uniform bool useHardwareRandomizer)
{
    uniform CircuitExplorerAdvancedRenderer* uniform self =
        (uniform CircuitExplorerAdvancedRenderer * uniform) _self;

    // OSPRay
    self->super.super.super.spp = spp;

    // Abstract
    self->super.super.bgMaterial =
        (uniform CircuitExplorerMaterial * uniform) bgMaterial;

    self->super.super.lights = (const uniform Light* uniform* uniform)lights;
    self->super.super.numLights = numLights;

    self->super.super.timestamp = timestamp;
    self->super.super.maxBounces = maxBounces;
    self->super.super.exposure = exposure;
    self->super.super.useHardwareRandomizer = useHardwareRandomizer;

    // Simulation
    self->super.secondaryModel = (uniform Model * uniform) secondaryModel;
    self->super.maxDistanceToSecondaryModel = maxDistanceToSecondaryModel;

    self->super.simulationData = (uniform float* uniform)simulationData;
    self->super.simulationDataSize = simulationDataSize;

    self->super.fogThickness = fogThickness;
    self->super.fogStart = fogStart;

    // Advanced
    self->shadows = shadows;
    self->softShadows = softShadows;
    self->softShadowsSamples = softShadowsSamples;

    self->giStrength = giStrength;
    self->giDistance = giDistance;
    self->giSamples = giSamples;
    self->epsilonFactor = epsilonFactor;

    self->randomNumber = randomNumber;

    self->samplingThreshold = samplingThreshold;

    self->volumeSamplesPerRay = volumeSamplesPerRay;
    self->volumeSpecularExponent = volumeSpecularExponent;
    self->volumeAlphaCorrection = volumeAlphaCorrection;

    self->clipPlanes = clipPlanes;
    self->numClipPlanes = numClipPlanes;
}
