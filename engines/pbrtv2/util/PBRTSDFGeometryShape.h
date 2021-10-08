/* Copyright (c) 2020, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#pragma once

#include <brayns/common/geometry/SDFGeometry.h>
#include <brayns/common/log.h>
#include <brayns/engine/Model.h>

#include <pbrtv2/core/montecarlo.h>
#include <pbrtv2/core/pbrt.h>
#include <pbrtv2/core/shape.h>

#include <algorithm>
#define _USE_MATH_DEFINES
#include <cmath>

#include "../PBRTConstants.h"

#define PBRT_MAX_MARCHING_ITERATIONS 32
#define PBRT_MAX_MARCH_FIX_ITERATIONS 16
#define PBRT_SDF_EPSILON 0.0001f
#define PBRT_SDF_BLEND_FACTOR 0.1f
#define PBRT_SDF_BLEND_LERP_FACTOR 0.2f
#define PBRT_ULP_EPSILON 1e-4
#define PBRT_MAX_NORMAL_FIX_ITERATIONS 6
#define PBRT_SURFACE_DISTANCE_TRESHOLD 2.0
#define PBRT_M_PI static_cast<float>(M_PI)

namespace brayns
{
inline float reduceMax(const Vector4f& v)
{
    return std::max(std::max(v.x, v.y), std::max(v.z, v.w));
}

inline float reduceMax(const Vector3f& v)
{
    return std::max(std::max(v.x, v.y), v.z);
}

inline Vector3f abs(const Vector3f& v)
{
    return Vector3f(fabs(static_cast<double>(v.x)),
                    fabs(static_cast<double>(v.y)),
                    fabs(static_cast<double>(v.z)));
}

inline float calcEpsilon(const Vector3f& p, const float t)
{
    return reduceMax(Vector4f(abs(p), t)) * PBRT_ULP_EPSILON;
}

inline float smoothstep(const float x)
{
    return x * x * (3 - 2 * x);
}

// https://en.wikipedia.org/wiki/Smoothstep
inline float smootherstep(const float x)
{
    return x * x * x * (x * (x * 6 - 15) + 10);
}

// polynomial smooth min (k = 0.1);
inline float sminPoly(const float a, const float b, const float k)
{
    const float h = glm::clamp(0.5f + 0.5f * (b - a) / k, 0.f, 1.f);
    return glm::lerp(b, a, h) - k * h * (1.f - h);
}

inline float sdSphere(const Vector3f& p, const Vector3f& c, float r)
{
    return glm::length(p - c) - r;
}

inline float sdCapsule(const Vector3f& p, const Vector3f& a, const Vector3f& b,
                       float r)
{
    const Vector3f pa = p - a, ba = b - a;
    const float h = glm::clamp(glm::dot(pa, ba) / glm::dot(ba, ba), 0.f, 1.f);
    return length(pa - ba * h) - r;
}

inline float sdConePill(const Vector3f& p, const Vector3f& p0,
                        const Vector3f& p1, const float r0, const float r1,
                        const bool useSigmoid)
{
    const Vector3f v = (p1 - p0);
    const Vector3f w = (p - p0);

    // distance to p0 along cone axis
    const float c1 = glm::dot(w, v);
    if (c1 <= 0)
    {
        return glm::length(p - p0) - r0;
    }

    // cone length
    const float c2 = glm::dot(v, v);
    if (c2 <= c1)
    {
        return glm::length(p - p1) - r1;
    }

    const float b = c1 / c2;
    const Vector3f Pb = p0 + b * v;

    const float thickness =
        useSigmoid ? glm::lerp(r0, r1, smootherstep(b)) : glm::lerp(r0, r1, b);

    return glm::length(p - Pb) - thickness;
}

inline float calcDistance(const SDFGeometry& primitive, const Vector3f& p)
{
    switch (primitive.type)
    {
    case SDFType::Sphere:
        return sdSphere(p, primitive.p0, primitive.r0);
    case SDFType::Pill:
        return sdCapsule(p, primitive.p0, primitive.p1, primitive.r0);
    case SDFType::ConePill:
        return sdConePill(p, primitive.p0, primitive.p1, primitive.r0,
                          primitive.r1, false);
    case SDFType::ConePillSigmoid:
        return sdConePill(p, primitive.p0, primitive.p1, primitive.r0,
                          primitive.r1, true);
    }

    return -1.0;
}

template <SDFType T>
class PBRTSDFGeometryShape : public pbrt::Shape
{
public:
    PBRTSDFGeometryShape(const pbrt::Transform* objToWorld,
                         const pbrt::Transform* worldToObj, bool revOrientation,
                         const SDFGeometry* geom, const SDFGeometryData* data)
        : pbrt::Shape(objToWorld, worldToObj, revOrientation)
        , _srcGeom(geom)
        , _srcData(data)
    {
        if (!_srcGeom || !_srcData)
            throw std::runtime_error(
                "PBRTSDFGeometryShape: Null sdf geometry given");

        const auto bbounds = getSDFBoundingBox(*_srcGeom);
        const auto& bmin = bbounds.getMin();
        const auto& bmax = bbounds.getMax();
        const pbrt::Point min(static_cast<float>(bmin.x),
                              static_cast<float>(bmin.y),
                              static_cast<float>(bmin.z));
        const pbrt::Point max(static_cast<float>(bmax.x),
                              static_cast<float>(bmax.y),
                              static_cast<float>(bmax.z));
        _bounds.pMin = min;
        _bounds.pMax = max;
    }

    ~PBRTSDFGeometryShape() {}

    pbrt::BBox ObjectBound() const final { return _bounds; }

    pbrt::BBox WorldBound() const final { return _bounds; }

    bool Intersect(const pbrt::Ray& ray, float* tHit, float* rayEpsilon,
                   pbrt::DifferentialGeometry* isect) const final
    {
        *tHit = raymarch(ray);
        if (*tHit > ray.mint && *tHit < ray.maxt)
        {
            const auto bOrg = Vector3f(ray.o.x, ray.o.y, ray.o.z);
            const auto bDir =
                glm::normalize(Vector3f(ray.d.x, ray.d.y, ray.d.z));
            const auto bPos = bOrg + (*tHit * bDir);
            const auto bNormal = computeNormal(bPos, calcEpsilon(bOrg, *tHit));
            const pbrt::Point pHit = ray(static_cast<float>(*tHit));
            *rayEpsilon = 1e-3f * *tHit;

            const pbrt::Normal pNormal(bNormal.x, bNormal.y, bNormal.z);

            pbrt::Vector dpdu, dpdv;
            pbrt::Normal dndu, dndv;
            float tu = 0.f, tv = 0.f;

            dndu = dndv = pbrt::Normal(0.f, 0.f, 0.f);
            pbrt::CoordinateSystem(pbrt::Vector(pNormal.x, pNormal.y,
                                                pNormal.z),
                                   &dpdu, &dpdv);

            *isect = pbrt::DifferentialGeometry(pHit, dpdu, dpdv, dndu, dndv,
                                                tu, tv, this);

            isect->nn = pNormal;

            return true;
        }

        return false;
    }

    bool IntersectP(const pbrt::Ray& ray) const final
    {
        const float t = raymarch(ray);
        return t > ray.mint && t < ray.maxt;
    }

    float Area() const final;
    pbrt::Point Sample(float u, float v, pbrt::Normal* pdf) const final;

    bool Projects(const pbrt::Point& p, pbrt::Point& ps,
                  pbrt::Normal& ns) const final
    {
        (void)p;
        (void)ps;
        (void)ns;
        return false;
    }

    const SDFGeometry& getSDFGeometry() const { return *_srcGeom; }
    const SDFGeometryData& getSDFGeometryData() const { return *_srcData; }

private:
    float shapeDistance(const Vector3f& p) const;

    float sdfDistance(const Vector3f& p) const
    {
        float d = shapeDistance(p);

        // TODO don't blend soma if far enough from eye
        const auto nindx = _srcGeom->neighboursIndex;
        const auto& neighbors = _srcData->neighbours[nindx];
        const auto numNeigh = neighbors.size();

        if (numNeigh > 0)
        {
            const float r0 = _srcGeom->r0;

            for (uint32_t i = 0; i < numNeigh; i++)
            {
                const SDFGeometry& neighbour =
                    _srcData->geometries[neighbors[i]];

                const float dOther = calcDistance(neighbour, p);
                const float r1 = neighbour.r0;
                const float blendFactor =
                    glm::lerp(std::min(r0, r1), std::max(r0, r1),
                              PBRT_SDF_BLEND_LERP_FACTOR);

                d = sminPoly(dOther, d, blendFactor * PBRT_SDF_BLEND_FACTOR);
            }
        }

        return d;
    }

    /**
     * Computes the normal vector at the SDF surface point given by pos by
     * applying the tetrahedron technique
     * http://iquilezles.org/www/articles/normalsSDF/normalsSDF.htm
     */
    Vector3f computeNormal(const Vector3f& pos, const float e) const
    {
        // tetrahedron technique (4 evaluations)
        static const Vector3f k0 = Vector3f(1, -1, -1),
                              k1 = Vector3f(-1, -1, 1),
                              k2 = Vector3f(-1, 1, -1), k3 = Vector3f(1, 1, 1);

        auto temp = glm::normalize(
            k0 * sdfDistance(pos + e * k0) + k1 * sdfDistance(pos + e * k1) +
            k2 * sdfDistance(pos + e * k2) + k3 * sdfDistance(pos + e * k3));

        // Fix it due precission problems (TODO check when PBRT_FLOAT is double
        // precission)
        auto test = glm::dot(temp, temp);
        auto tempE = e;
        auto i = 0;
        while (glm::isnan(test) && i < PBRT_MAX_NORMAL_FIX_ITERATIONS)
        {
            tempE *= 10.f;
            temp = glm::normalize(k0 * sdfDistance(pos + tempE * k0) +
                                  k1 * sdfDistance(pos + tempE * k1) +
                                  k2 * sdfDistance(pos + tempE * k2) +
                                  k3 * sdfDistance(pos + tempE * k3));
            test = glm::dot(temp, temp);
            ++i;
        }

        if (glm::isnan(glm::abs(test)))
            throw std::runtime_error(
                "PBRTSDFGeometryShape: Tetrahedron evaluation for normal "
                "failed");

        return temp;
    }

    /**
     * Utility raymarch function used to fix rays that are born in the surface
     * of the SDF. The standard raymarch approach generates an infinite loop for
     * sub surface materials and create artifacts for reflections.
     */
    float raymarchFixStep(const pbrt::Ray& ray, const float marchLength) const
    {
        const Vector3f rayO(ray.o.x, ray.o.y, ray.o.z);
        // const Vector3f rayD (ray.d.x, ray.d.y, ray.d.z);
        const Vector3f rayD =
            glm::normalize(Vector3f(ray.d.x, ray.d.y, ray.d.z));

        const auto stepLength =
            marchLength / static_cast<float>(PBRT_MAX_MARCHING_ITERATIONS);

        float lastSign = sdfDistance(rayO) < 0.f ? -1.f : 1.f;
        float lastT = 0.f;

        for (int i = 0; i < PBRT_MAX_MARCHING_ITERATIONS; ++i)
        {
            auto currentT = stepLength * static_cast<float>(i + 1);
            auto d = sdfDistance(rayO + rayD * currentT);
            auto sign = d < 0.f ? -1.f : 1.f;

            // We have cross the surface, refine hit and return distance
            // i > 0 to allow surface rays penetrate the surface
            if (sign != lastSign && i > 5)
            {
                const auto fixLen = currentT - lastT;
                const auto fixStep =
                    fixLen / static_cast<float>(PBRT_MAX_MARCH_FIX_ITERATIONS);
                for (int j = 0; j < PBRT_MAX_MARCH_FIX_ITERATIONS; j++)
                {
                    currentT = lastT + (fixStep * static_cast<float>(i + 1));
                    d = sdfDistance(rayO + rayD * currentT);
                    sign = d < 0.f ? -1.f : 1.f;

                    if (sign != lastSign)
                        return currentT;
                }

                return currentT;
            }

            lastSign = sign;
            lastT = currentT;
        }

        return 0.f;
    }

    float raymarch(const pbrt::Ray& ray) const
    {
        float bbHit0 = 0.f, bbHit1 = 0.f;
        if (!_bounds.IntersectP(ray, &bbHit0, &bbHit1))
            return -1.f;

        const Vector3f rayO(ray.o.x, ray.o.y, ray.o.z);
        const Vector3f rayD =
            Vector3f(ray.d.x, ray.d.y,
                     ray.d.z); // glm::normalize(Vector3f(ray.d.x,
                               // ray.d.y, ray.d.z));
        const auto initialDistance = sdfDistance(rayO);
        const float sdfSign = initialDistance < 0.f ? -1.f : 1.f;

        float t = bbHit0;
        const float stepLength =
            (bbHit1 - bbHit0) /
            static_cast<float>(PBRT_MAX_MARCHING_ITERATIONS);

        for (int i = 0; i < PBRT_MAX_MARCHING_ITERATIONS; ++i)
        {
            // Current position to evaluate
            const auto currentT =
                bbHit0 + stepLength * static_cast<float>(i + 1);
            const Vector3f p = rayO + rayD * currentT;

            // Current distance to closest point of the shape
            float currentDist = sdfDistance(p);
            float currentSign = currentDist < 0.f ? -1.f : 1.f;

            // We cross the surface, refine hit and return
            if (currentSign != sdfSign)
            {
                auto start = t;
                auto end = currentT;

                for (int j = 0; j < PBRT_MAX_MARCHING_ITERATIONS; ++j)
                {
                    auto midPoint = (end + start) * 0.5f;
                    auto pFix = rayO + rayD * midPoint;
                    currentDist = sdfDistance(pFix);
                    currentSign = currentDist < 0.f ? -1.f : 1.f;

                    if (currentSign != sdfSign)
                        end = midPoint;
                    else
                        start = midPoint;
                }

                return start > bbHit1 ? -1.f : start;
                return -1.f;
            }

            t = currentT;
        }

        // We only reach this point if there is no surface cross,
        // Which means we didnt hit it
        return -1.f;
    }

private:
    const SDFGeometry* const _srcGeom;
    const SDFGeometryData* const _srcData;
    pbrt::BBox _bounds;
};

template <>
float PBRTSDFGeometryShape<SDFType::Sphere>::shapeDistance(
    const Vector3f& p) const
{
    return sdSphere(p, _srcGeom->p0, _srcGeom->r0);
}

template <>
float PBRTSDFGeometryShape<SDFType::Pill>::shapeDistance(
    const Vector3f& p) const
{
    return sdCapsule(p, _srcGeom->p0, _srcGeom->p1, _srcGeom->r0);
}

template <>
float PBRTSDFGeometryShape<SDFType::ConePill>::shapeDistance(
    const Vector3f& p) const
{
    return sdConePill(p, _srcGeom->p0, _srcGeom->p1, _srcGeom->r0, _srcGeom->r1,
                      false);
}

template <>
float PBRTSDFGeometryShape<SDFType::ConePillSigmoid>::shapeDistance(
    const Vector3f& p) const
{
    return sdConePill(p, _srcGeom->p0, _srcGeom->p1, _srcGeom->r0, _srcGeom->r1,
                      true);
}

template <>
float PBRTSDFGeometryShape<SDFType::Sphere>::Area() const
{
    return PBRT_M_PI * 4.0f * _srcGeom->r0 * _srcGeom->r0;
}

template <>
float PBRTSDFGeometryShape<SDFType::Pill>::Area() const
{
    const auto h = glm::length(_srcGeom->p1 - _srcGeom->p0);

    // const auto sphere = pbrt::Pi * 4.0 * geom.r0 * geom.r0;
    // const auto cylinder = pbrt::Pi * 2.0 * geom.r0 * h;

    return PBRT_M_PI * 2.f * _srcGeom->r0 * (_srcGeom->r0 * 2.f + h);
}

template <>
float PBRTSDFGeometryShape<SDFType::ConePill>::Area() const
{
    const auto h = glm::length(_srcGeom->p1 - _srcGeom->p0);
    const auto s = sqrtf(
        (_srcGeom->r1 - _srcGeom->r0) * (_srcGeom->r1 - _srcGeom->r0) + h * h);
    // const auto cone = pbrt::Pi * (geom.r1 + geom.r0) * s;
    // const auto sphereA = pbrt::Pi * 2.0 * geom.r0;
    // const auto sphereB = pbrt::Pi * 2.0 * geom.r1;

    return PBRT_M_PI * (_srcGeom->r0 + _srcGeom->r1) * (2.0f + s);
}

template <>
float PBRTSDFGeometryShape<SDFType::ConePillSigmoid>::Area() const
{
    const auto h = glm::length(_srcGeom->p1 - _srcGeom->p0);
    const auto s = sqrtf(
        (_srcGeom->r1 - _srcGeom->r0) * (_srcGeom->r1 - _srcGeom->r0) + h * h);

    return PBRT_M_PI * (_srcGeom->r0 + _srcGeom->r1) * (2.f + s);
}

template <>
pbrt::Point PBRTSDFGeometryShape<SDFType::Sphere>::Sample(
    float u, float v, pbrt::Normal* normal) const
{
    const auto center =
        pbrt::Point(_srcGeom->p0.x, _srcGeom->p0.y, _srcGeom->p0.z);
    const auto sample = _srcGeom->r0 * pbrt::UniformSampleSphere(u, v);
    pbrt::Point p = center + sample;
    p *= (_srcGeom->r0 / pbrt::Distance(p, center)); // Reproject
    *normal = pbrt::Normalize(pbrt::Normal(sample));

    return p;
}

template <>
pbrt::Point PBRTSDFGeometryShape<SDFType::Pill>::Sample(
    float u, float v, pbrt::Normal* normal) const
{
    // Determine were are we in the pill (top half sphere, cylinder, bottom half
    // sphere)
    const auto h = glm::length(_srcGeom->p1 - _srcGeom->p0);
    const auto totalLen = _srcGeom->r0 * 2.f + h;
    const auto sampledLen = totalLen * u;

    pbrt::Point p;

    if (sampledLen <= _srcGeom->r0) // We are in top sphere
    {
        const auto bDir = glm::normalize(_srcGeom->p0 - _srcGeom->p1);
        const pbrt::Vector pDir =
            pbrt::Normalize(pbrt::Vector(bDir.x, bDir.y, bDir.z));

        const auto sample = pbrt::UniformSampleHemisphere(u, v);
        auto sampleDir =
            pbrt::Vector(_srcGeom->p0.x, _srcGeom->p0.y, _srcGeom->p0.z) +
            _srcGeom->r0 * sample;

        if (pbrt::Dot(pDir, pbrt::Normalize(sampleDir)) < 0.f)
            sampleDir = -sampleDir;

        p = pbrt::Point(sampleDir.x, sampleDir.y, sampleDir.z);
        *normal = pbrt::Normal(pbrt::Normalize(sampleDir));
    }
    else if (sampledLen > _srcGeom->r0 &&
             sampledLen <= _srcGeom->r0 + h) // In cylinder
    {
        // Find a vector perpendicular to the pill main direction
        const auto bDir = glm::normalize(_srcGeom->p1 - _srcGeom->p0);
        auto v1 = 1.f, v2 = 1.f;
        auto v3 = (-bDir.x - bDir.y) / (bDir.z);
        const auto ortho = glm::normalize(glm::vec3(v1, v2, v3));

        // Rotate it based on the given sample
        const auto phi = v * 2.f * PBRT_M_PI;
        auto rotated = glm::rotate(ortho, static_cast<float>(phi), bDir);

        // Use the perpendicular vector to sample a point in the
        // circle at the bottom of the cylinder
        auto sample = _srcGeom->p0 + rotated * _srcGeom->r0;

        // Move it along cylinder axis
        auto normaHeight = (sampledLen - _srcGeom->r0) / h;
        sample += (bDir * normaHeight);

        p = pbrt::Point(sample.x, sample.y, sample.z);
        *normal =
            pbrt::Normalize(pbrt::Normal(rotated.x, rotated.y, rotated.z));
    }
    else // In bottom sphere
    {
        const auto bDir = glm::normalize(_srcGeom->p1 - _srcGeom->p0);
        const pbrt::Vector pDir =
            pbrt::Normalize(pbrt::Vector(bDir.x, bDir.y, bDir.z));

        const auto sample = pbrt::UniformSampleHemisphere(u, v);
        auto sampleDir =
            pbrt::Vector(_srcGeom->p1.x, _srcGeom->p1.y, _srcGeom->p1.z) +
            _srcGeom->r0 * sample;

        if (pbrt::Dot(pDir, pbrt::Normalize(sampleDir)) < 0.f)
            sampleDir = -sampleDir;

        p = pbrt::Point(sampleDir.x, sampleDir.y, sampleDir.z);
        *normal = pbrt::Normal(pbrt::Normalize(sampleDir));
    }

    return p;
}

template <>
pbrt::Point PBRTSDFGeometryShape<SDFType::ConePill>::Sample(
    float u, float v, pbrt::Normal* normal) const
{
    // Determine were are we in the pill (top half sphere, cylinder, bottom half
    // sphere)
    const auto h = glm::length(_srcGeom->p1 - _srcGeom->p0);
    const auto totalLen = _srcGeom->r0 * 2.f + h;
    const auto sampledLen = totalLen * u;

    pbrt::Point p;

    if (sampledLen <= _srcGeom->r0) // We are in top sphere
    {
        const auto bDir = glm::normalize(_srcGeom->p0 - _srcGeom->p1);
        const pbrt::Vector pDir =
            pbrt::Normalize(pbrt::Vector(bDir.x, bDir.y, bDir.z));

        const auto sample = pbrt::UniformSampleHemisphere(u, v);
        auto sampleDir =
            pbrt::Vector(_srcGeom->p0.x, _srcGeom->p0.y, _srcGeom->p0.z) +
            _srcGeom->r0 * sample;

        if (pbrt::Dot(pDir, pbrt::Normalize(sampleDir)) < 0.f)
            sampleDir = -sampleDir;

        p = pbrt::Point(sampleDir.x, sampleDir.y, sampleDir.z);
        *normal = pbrt::Normal(pbrt::Normalize(sampleDir));
    }
    else if (sampledLen > _srcGeom->r0 &&
             sampledLen <= _srcGeom->r0 + h) // In frustrum cone
    {
        // Find a vector perpendicular to the pill main direction
        const auto bDir = glm::normalize(_srcGeom->p1 - _srcGeom->p0);
        auto v1 = 1.f, v2 = 1.f;
        auto v3 = (-bDir.x - bDir.y) / (bDir.z);
        const auto ortho = glm::normalize(glm::vec3(v1, v2, v3));

        // Rotate it based on the given sample
        const auto phi = v * 2.f * PBRT_M_PI;
        auto rotated = glm::rotate(ortho, static_cast<float>(phi), bDir);

        // Adjust radius based on height
        auto normaHeight = (sampledLen - _srcGeom->r0) / h;
        const auto rad = glm::lerp(_srcGeom->r0, _srcGeom->r1, normaHeight);
        auto sample = _srcGeom->p0 + rotated * rad;

        // Adjust position along the fustrum cone
        sample += (bDir * normaHeight);

        p = pbrt::Point(rotated.x, rotated.y, rotated.z);
        *normal =
            pbrt::Normalize(pbrt::Normal(rotated.x, rotated.y, rotated.z));
    }
    else // In bottom sphere
    {
        const auto bDir = glm::normalize(_srcGeom->p1 - _srcGeom->p0);
        const pbrt::Vector pDir =
            pbrt::Normalize(pbrt::Vector(bDir.x, bDir.y, bDir.z));

        const auto sample = pbrt::UniformSampleHemisphere(u, v);
        auto sampleDir =
            pbrt::Vector(_srcGeom->p1.x, _srcGeom->p1.y, _srcGeom->p1.z) +
            _srcGeom->r0 * sample;

        if (pbrt::Dot(pDir, pbrt::Normalize(sampleDir)) < 0.f)
            sampleDir = -sampleDir;

        p = pbrt::Point(sampleDir.x, sampleDir.y, sampleDir.z);
        *normal = pbrt::Normal(pbrt::Normalize(sampleDir));
    }

    return p;
}

template <>
pbrt::Point PBRTSDFGeometryShape<SDFType::ConePillSigmoid>::Sample(
    float u, float v, pbrt::Normal* normal) const
{
    pbrt::Point p;

    // Determine were are we in the pill (top half sphere, cylinder, bottom half
    // sphere)
    const auto h = glm::length(_srcGeom->p1 - _srcGeom->p0);
    const auto totalLen = _srcGeom->r0 * 2.f + h;
    const auto sampledLen = totalLen * u;

    if (sampledLen <= _srcGeom->r0) // We are in top sphere
    {
        const auto bDir = glm::normalize(_srcGeom->p0 - _srcGeom->p1);
        const pbrt::Vector pDir =
            pbrt::Normalize(pbrt::Vector(bDir.x, bDir.y, bDir.z));

        const auto sample = pbrt::UniformSampleHemisphere(u, v);
        auto sampleDir =
            pbrt::Vector(_srcGeom->p0.x, _srcGeom->p0.y, _srcGeom->p0.z) +
            _srcGeom->r0 * sample;

        if (pbrt::Dot(pDir, pbrt::Normalize(sampleDir)) < 0.f)
            sampleDir = -sampleDir;

        p = pbrt::Point(sampleDir.x, sampleDir.y, sampleDir.z);
        *normal = pbrt::Normal(pbrt::Normalize(sampleDir));
    }
    else if (sampledLen > _srcGeom->r0 &&
             sampledLen <= _srcGeom->r0 + h) // In frustrum cone
    {
        // Find a vector perpendicular to the pill main direction
        const auto bDir = glm::normalize(_srcGeom->p1 - _srcGeom->p0);
        auto v1 = 1.f, v2 = 1.f;
        auto v3 = (-bDir.x - bDir.y) / (bDir.z);
        const auto ortho = glm::normalize(glm::vec3(v1, v2, v3));

        // Rotate it based on the given sample
        const auto phi = v * 2.f * PBRT_M_PI;
        auto rotated = glm::rotate(ortho, static_cast<float>(phi), bDir);

        // Adjust radius based on height
        auto normaHeight = (sampledLen - _srcGeom->r0) / h;
        const auto rad = glm::lerp(_srcGeom->r0, _srcGeom->r1, normaHeight);
        auto sample = _srcGeom->p0 + rotated * rad;

        // Adjust position along the fustrum cone
        sample += (bDir * normaHeight);

        p = pbrt::Point(rotated.x, rotated.y, rotated.z);
        *normal =
            pbrt::Normalize(pbrt::Normal(rotated.x, rotated.y, rotated.z));
    }
    else // In bottom sphere
    {
        const auto bDir = glm::normalize(_srcGeom->p1 - _srcGeom->p0);
        const pbrt::Vector pDir =
            pbrt::Normalize(pbrt::Vector(bDir.x, bDir.y, bDir.z));

        const auto sample = pbrt::UniformSampleHemisphere(u, v);
        auto sampleDir =
            pbrt::Vector(_srcGeom->p1.x, _srcGeom->p1.y, _srcGeom->p1.z) +
            _srcGeom->r0 * sample;

        if (pbrt::Dot(pDir, pbrt::Normalize(sampleDir)) < 0.f)
            sampleDir = -sampleDir;

        p = pbrt::Point(sampleDir.x, sampleDir.y, sampleDir.z);
        *normal = pbrt::Normal(pbrt::Normalize(sampleDir));
    }

    return p;
}
} // namespace brayns

