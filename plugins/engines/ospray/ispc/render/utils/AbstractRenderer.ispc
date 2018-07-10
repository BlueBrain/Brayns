/* Copyright (c) 2015-2017, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
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

#include <plugins/engines/ospray/ispc/render/utils/AbstractRenderer.ih>

inline bool launchRandomRay(
    const uniform AbstractRenderer* uniform self, varying ScreenSample& sample,
    const varying vec3f& intersection, const varying vec3f& normal,
    DifferentialGeometry& geometry, varying vec3f& backgroundColor,
    varying float& distanceToIntersection, varying vec3f& randomDirection)
{
    randomDirection = getRandomVector(sample, normal, self->randomNumber);
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

    traceRay(self->super.model, randomRay);
    if (randomRay.geomID < 0)
    {
        // No intersection
        backgroundColor = make_vec3f(
            skyboxMapping((Renderer*)self, randomRay, self->bgMaterial));
        return false;
    }

    // Random ray hits a primitive
    distanceToIntersection = randomRay.t * randomRay.t;
    postIntersect(self->super.model, geometry, randomRay,
                  DG_NG | DG_NS | DG_NORMALIZE | DG_MATERIALID | DG_COLOR |
                      DG_TEXCOORD);
    return true;
}

inline vec3f refractedVector(const varying vec3f& direction,
                             const varying vec3f& normal,
                             const varying float n1, const varying float n2)
{
    if (n2 == 0.f)
        return direction;
    const float eta = n1 / n2;
    const float cos1 = -dot(direction, normal);
    const float cos2 = 1.f - eta * eta * (1.f - cos1 * cos1);
    if (cos2 > 0.f)
        return eta * direction + (eta * cos1 - sqrt(cos2)) * normal;
    return direction;
}

inline void indirectShading(const uniform AbstractRenderer* uniform self,
                            varying ScreenSample& sample,
                            const varying vec3f& intersection,
                            const varying vec3f& normal,
                            DifferentialGeometry& geometry,
                            varying vec3f& indirectShadingColor,
                            varying float& indirectShadingPower)
{
    vec3f backgroundColor;
    float distanceToIntersection = infinity;
    indirectShadingPower = 0.f;

    // Launch a random ray
    vec3f randomDirection;
    if (launchRandomRay((AbstractRenderer*)self, sample, intersection, normal,
                        geometry, backgroundColor, distanceToIntersection,
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

inline float shadedLightIntensity(const uniform AbstractRenderer* uniform self,
                                  varying ScreenSample& sample,
                                  const varying vec3f& intersection,
                                  const varying vec3f& normal,
                                  const varying vec3f& lightDirection,
                                  const varying float epsilon)
{
    vec3f ld = lightDirection;
    if (self->softShadows != 0.f)
        // Slightly alter light direction for Soft shadows
        ld = normalize(ld +
                       self->softShadows *
                           getRandomVector(sample, normal, self->randomNumber));

    Ray shadowRay;
    setRay(shadowRay, intersection, ld);
    shadowRay.t0 = epsilon;
    shadowRay.time = sample.ray.time;
    shadowRay.t = infinity;

    varying float opacity = 0.f;
    varying float intensity = 1.f;
    varying int depth = 0;
    varying bool moreRebounds = true;

#if 0
    // Light attenuation altered by volume
    // TO BE DONE?
    if (self->volumeData)
    {
        const vec4f volumetricValue =
            getVolumeContribution(self, shadowRay, sample,
                                  self->volumeSamplesPerRay);
        intensity = volumetricValue.w;
        moreRebounds = (intensity > 0.f);
    }
#endif

    while (moreRebounds && depth < NB_MAX_REBOUNDS)
    {
        traceRay(self->super.model, shadowRay);

        if (shadowRay.geomID >= 0)
        {
            DifferentialGeometry dg;
            postIntersect(self->super.model, dg, shadowRay,
                          DG_MATERIALID | DG_TEXCOORD | DG_COLOR);

            uniform ExtendedOBJMaterial* objMaterial =
                (uniform ExtendedOBJMaterial*)dg.material;

            varying float materialOpacity = 1.f;

            if (objMaterial == 0)
                materialOpacity = 1.f - dg.color.w;
            else
            {
                foreach_unique(mat in objMaterial)
                {
                    materialOpacity = mat->d;
                    if (valid(mat->map_d))
                    {
                        vec4f d_map = get4f(mat->map_d, dg.st);
                        materialOpacity = d_map.x;
                    }
                    else if (valid(mat->map_Kd))
                    {
                        vec4f localDiffuseColor_map = get4f(mat->map_Kd, dg.st);
                        materialOpacity = 1.f - localDiffuseColor_map.w;
                    }
                }
            }

            opacity += materialOpacity;
            intensity -= materialOpacity * self->shadows;

            if (intensity < ALPHA_THRESHOLD || opacity >= 1.f)
                moreRebounds = false;

            shadowRay.t0 = shadowRay.t + dg.epsilon;
            shadowRay.t = infinity;
            shadowRay.primID = -1;
            shadowRay.geomID = -1;
            shadowRay.instID = -1;
            ++depth;
        }
        else
            moreRebounds = false;
    }
    return intensity;
}

inline void composite(const varying vec4f& src, varying vec4f& dst,
                      const varying float alphaCorrection)
{
    const float alpha =
        1.f - pow(1.f - min(src.w, 1.f - 1.f / 256.f), alphaCorrection);
    const float a = alpha * (1.f - dst.w);
    dst.x = dst.x + src.x * a;
    dst.y = dst.y + src.y * a;
    dst.z = dst.z + src.z * a;
    dst.w += (alpha * (1.f - dst.w));
}

// AABB-Ray intersection ( http://prideout.net/blog/?p=64 ).
inline varying bool intersectBox(const uniform AbstractRenderer* uniform self,
                                 const varying Ray& ray, const vec3f& aabbMin,
                                 const vec3f& aabbMax, varying float& t0,
                                 varying float& t1)
{
    const float epsilon = 0.0001f;
    Ray r = ray;
    // We need to avoid division by zero in "vec3 invR = 1.0 / r.Dir;"
    if (r.dir.x == 0)
        r.dir.x = epsilon;

    if (r.dir.y == 0)
        r.dir.y = epsilon;

    if (r.dir.z == 0)
        r.dir.z = epsilon;

    vec3f invR = 1.f / r.dir;
    vec3f tbot = invR * (aabbMin - r.org);
    vec3f ttop = invR * (aabbMax - r.org);
    vec3f tmin = min(ttop, tbot);
    vec3f tmax = max(ttop, tbot);
    vec2f t = make_vec2f(max(tmin.x, tmin.y), max(tmin.x, tmin.z));
    t0 = max(t.x, t.y);
    t = make_vec2f(min(tmax.x, tmax.y), min(tmax.x, tmax.z));
    t1 = min(t.x, t.y);
    return (t0 <= t1);
}

inline varying float getVolumeShadowContribution(
    const uniform AbstractRenderer* uniform self, const varying Ray& ray,
    varying ScreenSample& sample)
{
#if 0
    // Find volume intersections
    float t0, t1;
    vec3f aabbMin = self->volumeOffset;
    vec3f aabbMax =
        make_vec3f(self->volumeOffset) +
        make_vec3f(self->volumeDimensions) * self->volumeElementSpacing;
    if (!intersectBox(self, ray, aabbMin, aabbMax, t0, t1))
        return 0.f;

    // Ray marching from light source to voxel
    const float epsilon = max(0.01f, (t1 - t0) / self->volumeSamplesPerRay);
    const vec3i dimensions = self->volumeDimensions;
    const float random = getRandomValue(sample, self->randomNumber) * epsilon;
    float shadowIntensity = 0.f;
    for (float t = t1 - random; t > epsilon && shadowIntensity < 1.f;
         t -= epsilon)
    {
        vec3f point = ray.org + ray.dir * t;

        if (point.x >= 0.f && point.x < dimensions.x && point.y >= 0.f &&
            point.y < dimensions.y && point.z >= 0.f && point.z < dimensions.z)
        {
            uint64 index = (uint64)(
                (uint64)floor(point.x) + (uint64)floor(point.y) * dimensions.x +
                (uint64)floor(point.z) * dimensions.x * dimensions.y);

            const uint8 voxelValue = self->volumeData[index];

            const float normalizedValue =
                self->colorMapSize * (voxelValue - self->colorMapMinValue) /
                self->colorMapRange;
            const vec4f colorMapColor = self->colorMap[normalizedValue];
            shadowIntensity += colorMapColor.w;
        }
    }
    return shadowIntensity;
#else
return 0.f;
#endif
}

inline varying vec4f getVolumeContribution(
    Volume* uniform volume, const uniform AbstractRenderer* uniform self,
    const varying Ray& ray, varying ScreenSample& sample)
{
    //    if (!self->colorMap)
    //        return make_vec4f(0.f, 1.f, 0.f, 0.f);

    // Find volume intersections
    float t0, t1;
    //    vec3f aabbMin = self->volumeOffset;
    //    vec3f aabbMax =
    //        make_vec3f(self->volumeOffset) +
    //        make_vec3f(self->volumeDimensions) * self->volumeElementSpacing;
    intersectBox(ray, volume->boundingBox, t0, t1);
    // return make_vec4f(0.f, 0.f, 0.f, 0.f);

    // Ray marching
    vec4f pathColor = make_vec4f(0.f);
    const float epsilon = max(0.01f, (t1 - t0) / self->volumeSamplesPerRay);
    const float random = getRandomValue(sample, self->randomNumber) * epsilon;
    t0 -= random;
    t1 -= random;
    for (float t = t0; t < t1 && pathColor.w < 1.f; t += epsilon)
    {
        //        const vec3f point = ((ray.org + ray.dir * t) -
        //        self->volumeOffset) /
        //                            self->volumeElementSpacing;
        const vec3f point = ray.org + t * ray.dir;

        //        if (point.x >= 0.f && point.x < self->volumeDimensions.x &&
        //            point.y >= 0.f && point.y < self->volumeDimensions.y &&
        //            point.z >= 0.f && point.z < self->volumeDimensions.z)
        {
            const float sample = volume->sample(volume, point);

            // Look up the color associated with the volume sample.
            vec3f sampleColor = volume->transferFunction->getColorForValue(
                volume->transferFunction, sample);

            // Look up the opacity associated with the volume sample.
            const float sampleOpacity =
                volume->transferFunction->getOpacityForValue(
                    volume->transferFunction, sample);

            //            const uint64 index =
            //                (uint64)((uint64)floor(point.x) +
            //                         (uint64)floor(point.y) *
            //                         self->volumeDimensions.x +
            //                         (uint64)floor(point.z) *
            //                         self->volumeDimensions.x *
            //                             self->volumeDimensions.y);

            //            const uint8 voxelValue = self->volumeData[index];

            //            const float normalizedValue =
            //                (voxelValue - self->colorMapMinValue) /
            //                self->colorMapRange;

            //            // Voxel color and opacity
            //            vec4f colorMapColor;
            //            if (normalizedValue < 0.f)
            //                colorMapColor = self->colorMap[0];
            //            else if (normalizedValue > 1.f)
            //                colorMapColor = self->colorMap[self->colorMapSize
            //                - 1];
            //            else
            //                colorMapColor =
            //                    self->colorMap[self->colorMapSize *
            //                    normalizedValue];

            //            // Light emission intensity
            //            const vec4f emissionIntensity =
            //                make_vec4f(self->emissionIntensitiesMap[normalizedValue],
            //                0.f);
            //            vec4f voxelColor =
            //                max(make_vec4f(0.f), emissionIntensity +
            //                colorMapColor);

            //            // Shadow intensity
            //            float shadowIntensity = 0.f;
            //            vec4f giContribution = make_vec4f(0.f);
            //            if (self->shadows > 0.f && voxelColor.w > 0.01f)
            //            {
            //                // Determine global illumination contribution
            //                for (uniform int i = 0; self->lights && i <
            //                self->numLights;
            //                     ++i)
            //                {
            //                    const uniform Light* uniform light =
            //                    self->lights[i];
            //                    const varying vec2f s =
            //                        make_vec2f(1.f / self->randomNumber);
            //                    DifferentialGeometry dg;
            //                    dg.P = point;
            //                    const varying Light_SampleRes lightSample =
            //                        light->sample(light, dg, s);

            //                    Ray lightRay = ray;
            //                    if (self->softShadows > 0.f)
            //                        lightRay.dir = normalize(
            //                            lightSample.dir +
            //                            self->softShadows *
            //                                getRandomVector(sample,
            //                                lightSample.dir,
            //                                                self->randomNumber));
            //                    else
            //                        lightRay.dir = lightSample.dir;

            //                    lightRay.t =
            //                    length(make_vec3f(self->volumeDimensions));
            //                    lightRay.t0 = dg.epsilon;
            //                    lightRay.org = point;
            //                    lightRay.dir = lightSample.dir;

            //                    shadowIntensity +=
            //                        getVolumeShadowContribution(self,
            //                        lightRay, sample);
            //                }
            //                const float giAttenuation =
            //                    1.f - shadowIntensity * self->shadows;
            //                voxelColor.x = voxelColor.x * giAttenuation;
            //                voxelColor.y = voxelColor.y * giAttenuation;
            //                voxelColor.z = voxelColor.z * giAttenuation;
            //            }

            // Compose final voxel color
            vec4f voxelColor =
                clamp(sampleOpacity / volume->samplingRate) *
                make_vec4f(sampleColor.x, sampleColor.y, sampleColor.z, 1.0f);
            composite(voxelColor, pathColor, (float)self->volumeSamplesPerRay);
        }
    }
    return make_vec4f(min(1.f, pathColor.x), min(1.f, pathColor.y),
                      min(1.f, pathColor.z), min(1.f, pathColor.w));
}
