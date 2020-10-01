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

#ifndef PBRTSDFGEOMETRYSHAPE_H
#define PBRTSDFGEOMETRYSHAPE_H

#include <brayns/common/geometry/SDFGeometry.h>
#include <brayns/common/log.h>
#include <brayns/engine/Model.h>

#include <pbrt/core/pbrt.h>
#include <pbrt/core/sampling.h>
#include <pbrt/core/shape.h>

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
#define PBRT_M_PI static_cast<pbrt::Float>(M_PI)

namespace brayns
{
inline pbrt::Float reduceMax(const Vector4f& v)
{
    return std::max(std::max(v.x, v.y), std::max(v.z, v.w));
}

inline pbrt::Float reduceMax(const Vector3f& v)
{
    return std::max(std::max(v.x, v.y), v.z);
}

inline Vector3f abs(const Vector3f& v)
{
    return Vector3f(fabs(static_cast<double>(v.x)),
                    fabs(static_cast<double>(v.y)),
                    fabs(static_cast<double>(v.z)));
}

inline pbrt::Float calcEpsilon(const Vector3f& p, const pbrt::Float t)
{
    return reduceMax(Vector4f(abs(p), t)) * PBRT_ULP_EPSILON;
}

inline pbrt::Float smoothstep(const float x)
{
    return x * x * (3 - 2 * x);
}

// https://en.wikipedia.org/wiki/Smoothstep
inline pbrt::Float smootherstep(const pbrt::Float x)
{
    return static_cast<pbrt::Float>(x * x * x * (x * (x * 6 - 15) + 10));
}

// polynomial smooth min (k = 0.1);
inline pbrt::Float sminPoly(const pbrt::Float a, const pbrt::Float b, const pbrt::Float k)
{
    //const float h = glm::clamp(0.5f + 0.5f * (b - a) / k, 0.f, 1.f);
    //return static_cast<pbrt::Float>(glm::lerp(b, a, h) - k * h * (1.f - h));
    const pbrt::Float h = pbrt::Clamp(pbrt::Float(0.5) + pbrt::Float(0.5) * (b - a) / k,
                                      pbrt::Float(0),
                                      pbrt::Float(1));
    return pbrt::Lerp(h, b, a) - k * h * (pbrt::Float(1) - h);
}

inline pbrt::Float sdSphere(const Vector3f& p, const Vector3f& c, pbrt::Float r)
{
    return static_cast<pbrt::Float>(glm::length(p - c) - r);
}

inline pbrt::Float sdCapsule(const Vector3f& p, const Vector3f& a, const Vector3f& b, pbrt::Float r)
{
    const Vector3f pa = p - a, ba = b - a;
    const pbrt::Float h = glm::clamp(glm::dot(pa, ba) / glm::dot(ba, ba), 0.f, 1.f);
    return length(pa - ba * h) - r;
}

inline float sdConePill(const Vector3f& p, const Vector3f& p0, const Vector3f& p1,
                        const pbrt::Float r0, const pbrt::Float r1, const bool useSigmoid)
{
    const Vector3f v = (p1 - p0);
    const Vector3f w = (p - p0);

    // distance to p0 along cone axis
    const pbrt::Float c1 = glm::dot(w, v);
    if (c1 <= 0)
    {
        return glm::length(p - p0) - r0;
    }

    // cone length
    const pbrt::Float c2 = glm::dot(v, v);
    if (c2 <= c1)
    {
        return glm::length(p - p1) - r1;
    }

    const pbrt::Float b = c1 / c2;
    const Vector3f Pb = p0 + b * v;

    const pbrt::Float thickness = useSigmoid
                        ? glm::lerp(r0, r1, smootherstep(b))
                        : glm::lerp(r0, r1, b);

    return glm::length(p - Pb) - thickness;
}

inline pbrt::Float calcDistance(const SDFGeometry& primitive, const Vector3f& p)
{
    switch(primitive.type)
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


template<SDFType T>
class PBRTSDFGeometryShape : public pbrt::Shape
{
public:
    PBRTSDFGeometryShape(const pbrt::Transform* objToWorld, const pbrt::Transform* worldToObj,
                         bool revOrientation, const SDFGeometry* geom,
                         const SDFGeometryData* data)
      : pbrt::Shape(objToWorld, worldToObj, revOrientation)
      , _srcGeom(geom)
      , _srcData(data)
    {
      if(!_srcGeom || !_srcData)
        throw std::runtime_error("PBRTSDFGeometryShape: Null sdf geometry given");

      const auto bbounds = getSDFBoundingBox(*_srcGeom);
      const auto& bmin = bbounds.getMin();
      const auto& bmax = bbounds.getMax();
      const pbrt::Point3f min (static_cast<pbrt::Float>(bmin.x),
                               static_cast<pbrt::Float>(bmin.y),
                               static_cast<pbrt::Float>(bmin.z));
      const pbrt::Point3f max (static_cast<pbrt::Float>(bmax.x),
                               static_cast<pbrt::Float>(bmax.y),
                               static_cast<pbrt::Float>(bmax.z));
      _bounds.pMin = min;
      _bounds.pMax = max;
    }

    ~PBRTSDFGeometryShape()
    {
    }

    pbrt::Bounds3f ObjectBound() const final
    {
      return _bounds;
    }

    pbrt::Bounds3f WorldBound() const final
    {
      return _bounds;
    }

    bool Intersect(const pbrt::Ray& ray, pbrt::Float* tHit, pbrt::SurfaceInteraction* isect,
                   bool testAlphaTexture = true) const final
    {
      (void)testAlphaTexture;

      *tHit = raymarch(ray);
      if(*tHit > 0.f && *tHit < ray.tMax)
      {
          
          const auto bOrg = Vector3f(ray.o.x, ray.o.y, ray.o.z);
          const auto bDir = glm::normalize(Vector3f(ray.d.x, ray.d.y, ray.d.z));
          const auto bPos = bOrg + (*tHit * bDir);
          const auto bNormal = computeNormal(bPos, calcEpsilon(bOrg, *tHit));
          const pbrt::Point3f pHit = ray(static_cast<pbrt::Float>(*tHit));
          const pbrt::Vector3f pError = pbrt::gamma(5) * pbrt::Abs(static_cast<pbrt::Vector3f>(pHit));

          const pbrt::Normal3f pNormal (bNormal.x, bNormal.y, bNormal.z);

          pbrt::Vector3f dpdu, dpdv;
          pbrt::Normal3f dndu, dndv;
          pbrt::Point2f uv;

          dndu = dndv = pbrt::Normal3f(0.f, 0.f, 0.f);
          uv = pbrt::Point2f(0.f, 0.f);
	      pbrt::CoordinateSystem(pbrt::Vector3f(pNormal.x, pNormal.y, pNormal.z), &dpdu, &dpdv);

          *isect = pbrt::SurfaceInteraction(pHit, pError, uv, -ray.d, dpdu, dpdv,
                                            dndu, dndv, ray.time, this);
        
          isect->n = isect->shading.n = pNormal;

          return true;
      }

      return false;
    }

    bool IntersectP(const pbrt::Ray& ray, bool testAlphaTexture = true) const final
    {
        (void)testAlphaTexture;
        const pbrt::Float t = raymarch(ray);
        return t > 0.f && t < ray.tMax;
    }

    pbrt::Float Area() const final;
    pbrt::Interaction Sample(const pbrt::Point2f& u, pbrt::Float* pdf) const final;

    const SDFGeometry& getSDFGeometry() const { return *_srcGeom; }
    const SDFGeometryData& getSDFGeometryData() const { return *_srcData; }

private:
    pbrt::Float shapeDistance(const Vector3f& p) const;

    pbrt::Float sdfDistance(const Vector3f& p) const
    {
        pbrt::Float d = shapeDistance(p);

        // TODO don't blend soma if far enough from eye
        const auto nindx = _srcGeom->neighboursIndex;
        const auto& neighbors = _srcData->neighbours[nindx];
        const auto numNeigh = neighbors.size();

        if(numNeigh > 0)
        {
          const pbrt::Float r0 = _srcGeom->r0;

          for (uint32_t i = 0; i < numNeigh; i++)
          {
              const SDFGeometry& neighbour = _srcData->geometries[neighbors[i]];

              const pbrt::Float dOther = calcDistance(neighbour, p);
              const pbrt::Float r1 = neighbour.r0;
              const pbrt::Float blendFactor = pbrt::Lerp(pbrt::Float(PBRT_SDF_BLEND_LERP_FACTOR), std::min(r0, r1), std::max(r0, r1));
                  //glm::lerp(std::min(r0, r1), std::max(r0, r1), PBRT_SDF_BLEND_LERP_FACTOR);

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
    Vector3f computeNormal(const Vector3f& pos, const pbrt::Float e) const
    {
        // tetrahedron technique (4 evaluations)
        static const Vector3f k0 = Vector3f(1, -1, -1), k1 = Vector3f(-1, -1, 1),
                              k2 = Vector3f(-1, 1, -1), k3 = Vector3f(1, 1, 1);

        auto temp = glm::normalize(k0 * sdfDistance(pos + e * k0) +
                                   k1 * sdfDistance(pos + e * k1) +
                                   k2 * sdfDistance(pos + e * k2) +
                                   k3 * sdfDistance(pos + e * k3));

        // Fix it due precission problems (TODO check when PBRT_FLOAT is double precission) 
        auto test = glm::dot(temp, temp);
        auto tempE = e;
        auto i = 0;
        while(glm::isnan(test) && i < PBRT_MAX_NORMAL_FIX_ITERATIONS)
        {
          tempE *= 10.f;
          temp = glm::normalize(k0 * sdfDistance(pos + tempE * k0) +
                                k1 * sdfDistance(pos + tempE * k1) +
                                k2 * sdfDistance(pos + tempE * k2) +
                                k3 * sdfDistance(pos + tempE * k3));
          test = glm::dot(temp, temp);
          ++i;
        }

        if(glm::isnan(glm::abs(test)))
          throw std::runtime_error("PBRTSDFGeometryShape: Tetrahedron evaluation for normal failed");

        return temp;
    }

    /**
     * Raymarch algorithm
     */
    pbrt::Float raymarch(const pbrt::Ray& ray) const
    {
        pbrt::Float bbHit0 = 0.f, bbHit1 = 0.f;
        if(!_bounds.IntersectP(ray, &bbHit0, &bbHit1))
            return -1.f;

        const Vector3f rayO (ray.o.x, ray.o.y, ray.o.z);
        const Vector3f rayD = Vector3f(ray.d.x, ray.d.y, ray.d.z);
        const auto initialDistance = sdfDistance(rayO);
        const pbrt::Float sdfSign = initialDistance < 0.f? -1.f : 1.f;

        pbrt::Float t = bbHit0;
        const pbrt::Float stepLength = (bbHit1 - bbHit0) / 
                                       static_cast<pbrt::Float>(PBRT_MAX_MARCHING_ITERATIONS);

        for (int i = 0; i < PBRT_MAX_MARCHING_ITERATIONS; ++i)
        {
            // Current position to evaluate
            const auto currentT = bbHit0 + stepLength * static_cast<pbrt::Float>(i + 1);
            const Vector3f p = rayO + rayD * currentT;

            // Current distance to closest point of the shape
            pbrt::Float currentDist = sdfDistance(p);
            pbrt::Float currentSign = currentDist < 0.f? -1.f : 1.f;

            // We cross the surface, refine hit and return
            if(currentSign != sdfSign)
            {
                auto start = t;
                auto end = currentT;

                for (int j = 0; j < PBRT_MAX_MARCHING_ITERATIONS; ++j)
                {
                    auto midPoint = (end + start) * 0.5f;
                    auto pFix = rayO + rayD * midPoint;
                    currentDist = sdfDistance(pFix);
                    currentSign = currentDist < 0.f? -1.f : 1.f;

                    if(currentSign != sdfSign)
                        end = midPoint;
                    else
                        start = midPoint;
                }

                return start > bbHit1? -1.f : start;
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
    pbrt::Bounds3f _bounds;
};


template<>
pbrt::Float PBRTSDFGeometryShape<SDFType::Sphere>::shapeDistance(const Vector3f& p) const
{
    return sdSphere(p, _srcGeom->p0, _srcGeom->r0);
}

template<>
pbrt::Float PBRTSDFGeometryShape<SDFType::Pill>::shapeDistance(const Vector3f& p) const
{
    return sdCapsule(p, _srcGeom->p0, _srcGeom->p1, _srcGeom->r0);
}

template<>
pbrt::Float PBRTSDFGeometryShape<SDFType::ConePill>::shapeDistance(const Vector3f& p) const
{
    return sdConePill(p, _srcGeom->p0, _srcGeom->p1, _srcGeom->r0, _srcGeom->r1, false);
}

template<>
pbrt::Float PBRTSDFGeometryShape<SDFType::ConePillSigmoid>::shapeDistance(const Vector3f& p) const
{
    return sdConePill(p, _srcGeom->p0, _srcGeom->p1, _srcGeom->r0, _srcGeom->r1, true);
}

template<>
pbrt::Float PBRTSDFGeometryShape<SDFType::Sphere>::Area() const
{
    return PBRT_M_PI * 4.0f * _srcGeom->r0 * _srcGeom->r0;
}

template<>
pbrt::Float PBRTSDFGeometryShape<SDFType::Pill>::Area() const
{
    const auto h = glm::length(_srcGeom->p1 - _srcGeom->p0);

    //const auto sphere = pbrt::Pi * 4.0 * geom.r0 * geom.r0;
    //const auto cylinder = pbrt::Pi * 2.0 * geom.r0 * h;

    return PBRT_M_PI * 2.f * _srcGeom->r0 * (_srcGeom->r0 * 2.f + h);
}

template<>
pbrt::Float PBRTSDFGeometryShape<SDFType::ConePill>::Area() const
{
    const auto h = glm::length(_srcGeom->p1 - _srcGeom->p0);
    const auto s = sqrtf((_srcGeom->r1 - _srcGeom->r0)*(_srcGeom->r1 - _srcGeom->r0) + h*h);
    //const auto cone = pbrt::Pi * (geom.r1 + geom.r0) * s;
    //const auto sphereA = pbrt::Pi * 2.0 * geom.r0;
    //const auto sphereB = pbrt::Pi * 2.0 * geom.r1;

    return PBRT_M_PI * (_srcGeom->r0 + _srcGeom->r1) * (2.0f + s);
}

template<>
pbrt::Float PBRTSDFGeometryShape<SDFType::ConePillSigmoid>::Area() const
{
    const auto h = glm::length(_srcGeom->p1 - _srcGeom->p0);
    const auto s = sqrtf((_srcGeom->r1 - _srcGeom->r0)*(_srcGeom->r1 - _srcGeom->r0) + h*h);

    return PBRT_M_PI * (_srcGeom->r0 + _srcGeom->r1) * (2.f + s);
}

template<>
pbrt::Interaction 
PBRTSDFGeometryShape<SDFType::Sphere>::Sample(const pbrt::Point2f& u, 
                                              pbrt::Float* pdf) const
{
    pbrt::Interaction it;
    const auto center = pbrt::Point3f(_srcGeom->p0.x, _srcGeom->p0.y, _srcGeom->p0.z);
    const auto sample = _srcGeom->r0 * pbrt::UniformSampleSphere(u);
    it.p = center + sample;
    it.p *= (_srcGeom->r0 / pbrt::Distance(it.p, center)); // Reproject
    it.n = pbrt::Normalize(pbrt::Normal3f(sample));

    *pdf = 1.f / Area();

    return it;
}

template<>
pbrt::Interaction 
PBRTSDFGeometryShape<SDFType::Pill>::Sample(const pbrt::Point2f& u, 
                                            pbrt::Float* pdf) const
{
    pbrt::Interaction i;

    // Determine were are we in the pill (top half sphere, cylinder, bottom half sphere)
    const auto h = glm::length(_srcGeom->p1 - _srcGeom->p0);
    const auto totalLen = _srcGeom->r0 * 2.f + h;
    const auto sampledLen = totalLen * u[0];

    if(sampledLen <= _srcGeom->r0) // We are in top sphere
    {
        const auto bDir = glm::normalize(_srcGeom->p0 - _srcGeom->p1);
        const pbrt::Vector3f pDir = pbrt::Normalize(pbrt::Vector3f (bDir.x, bDir.y, bDir.z));

        const auto sample = pbrt::UniformSampleHemisphere(u);
        auto sampleDir = pbrt::Vector3f(_srcGeom->p0.x, _srcGeom->p0.y, _srcGeom->p0.z) + _srcGeom->r0 * sample;

        if(pbrt::Dot(pDir, pbrt::Normalize(sampleDir)) < 0.f)
            sampleDir = -sampleDir;

        i.p = pbrt::Point3f(sampleDir.x, sampleDir.y, sampleDir.z);
        i.n = pbrt::Normal3f(pbrt::Normalize(sampleDir));
    }
    else if (sampledLen > _srcGeom->r0 && sampledLen <= _srcGeom->r0 + h) // In cylinder
    {
        // Find a vector perpendicular to the pill main direction
        const auto bDir = glm::normalize(_srcGeom->p1 - _srcGeom->p0);
        auto v1 = 1.f, v2 = 1.f;
        auto v3 = (-bDir.x - bDir.y) / (bDir.z);
        const auto ortho = glm::normalize(glm::vec3(v1, v2, v3));

        // Rotate it based on the given sample
        const auto phi = u[1] * 2.f * PBRT_M_PI;
        auto rotated = glm::rotate(ortho, static_cast<float>(phi), bDir);

        // Use the perpendicular vector to sample a point in the
        // circle at the bottom of the cylinder
        auto sample = _srcGeom->p0 + rotated * _srcGeom->r0;

        // Move it along cylinder axis
        auto normaHeight = (sampledLen - _srcGeom->r0) / h;
        sample += (bDir * normaHeight);

        i.p = pbrt::Point3f(sample.x, sample.y, sample.z);
        i.n = pbrt::Normalize(pbrt::Normal3f(rotated.x, rotated.y, rotated.z));
    }
    else // In bottom sphere
    {
        const auto bDir = glm::normalize(_srcGeom->p1 - _srcGeom->p0);
        const pbrt::Vector3f pDir = pbrt::Normalize(pbrt::Vector3f (bDir.x, bDir.y, bDir.z));

        const auto sample = pbrt::UniformSampleHemisphere(u);
        auto sampleDir = pbrt::Vector3f(_srcGeom->p1.x, _srcGeom->p1.y, _srcGeom->p1.z) + _srcGeom->r0 * sample;

        if(pbrt::Dot(pDir, pbrt::Normalize(sampleDir)) < 0.f)
            sampleDir = -sampleDir;

        i.p = pbrt::Point3f(sampleDir.x, sampleDir.y, sampleDir.z);
        i.n = pbrt::Normal3f(pbrt::Normalize(sampleDir));
    }

    *pdf = 1.f / Area();

    return i;
}

template<>
pbrt::Interaction 
PBRTSDFGeometryShape<SDFType::ConePill>::Sample(const pbrt::Point2f& u, 
                                                pbrt::Float* pdf) const
{
    pbrt::Interaction i;

    // Determine were are we in the pill (top half sphere, cylinder, bottom half sphere)
    const auto h = glm::length(_srcGeom->p1 - _srcGeom->p0);
    const auto totalLen = _srcGeom->r0 * 2.f + h;
    const auto sampledLen = totalLen * u[0];

    if(sampledLen <= _srcGeom->r0) // We are in top sphere
    {
        const auto bDir = glm::normalize(_srcGeom->p0 - _srcGeom->p1);
        const pbrt::Vector3f pDir = pbrt::Normalize(pbrt::Vector3f (bDir.x, bDir.y, bDir.z));

        const auto sample = pbrt::UniformSampleHemisphere(u);
        auto sampleDir = pbrt::Vector3f(_srcGeom->p0.x, _srcGeom->p0.y, _srcGeom->p0.z) + _srcGeom->r0 * sample;

        if(pbrt::Dot(pDir, pbrt::Normalize(sampleDir)) < 0.f)
            sampleDir = -sampleDir;

        i.p = pbrt::Point3f(sampleDir.x, sampleDir.y, sampleDir.z);
        i.n = pbrt::Normal3f(pbrt::Normalize(sampleDir));
    }
    else if (sampledLen > _srcGeom->r0 && sampledLen <= _srcGeom->r0 + h) // In frustrum cone
    {
        // Find a vector perpendicular to the pill main direction
        const auto bDir = glm::normalize(_srcGeom->p1 - _srcGeom->p0);
        auto v1 = 1.f, v2 = 1.f;
        auto v3 = (-bDir.x - bDir.y) / (bDir.z);
        const auto ortho = glm::normalize(glm::vec3(v1, v2, v3));

        // Rotate it based on the given sample
        const auto phi = u[1] * 2.f * PBRT_M_PI;
        auto rotated = glm::rotate(ortho, static_cast<float>(phi), bDir);

        // Adjust radius based on height
        auto normaHeight = (sampledLen - _srcGeom->r0) / h;
        const auto rad = pbrt::Lerp(normaHeight, _srcGeom->r0, _srcGeom->r1);
        //const auto rad = glm::lerp(_srcGeom->r0, _srcGeom->r1, normaHeight);
        auto sample = _srcGeom->p0 + rotated * rad;

        // Adjust position along the fustrum cone
        sample += (bDir * normaHeight);

        i.p = pbrt::Point3f(rotated.x, rotated.y, rotated.z);
        i.n = pbrt::Normalize(pbrt::Normal3f(rotated.x, rotated.y, rotated.z));
    }
    else // In bottom sphere
    {
        const auto bDir = glm::normalize(_srcGeom->p1 - _srcGeom->p0);
        const pbrt::Vector3f pDir = pbrt::Normalize(pbrt::Vector3f (bDir.x, bDir.y, bDir.z));

        const auto sample = pbrt::UniformSampleHemisphere(u);
        auto sampleDir = pbrt::Vector3f(_srcGeom->p1.x, _srcGeom->p1.y, _srcGeom->p1.z) + _srcGeom->r0 * sample;

        if(pbrt::Dot(pDir, pbrt::Normalize(sampleDir)) < 0.f)
            sampleDir = -sampleDir;

        i.p = pbrt::Point3f(sampleDir.x, sampleDir.y, sampleDir.z);
        i.n = pbrt::Normal3f(pbrt::Normalize(sampleDir));
    }

    *pdf = 1.f / Area();

    return i;
}

template<>
pbrt::Interaction 
PBRTSDFGeometryShape<SDFType::ConePillSigmoid>::Sample(const pbrt::Point2f& u, 
                                                       pbrt::Float* pdf) const
{
    pbrt::Interaction i;

    // Determine were are we in the pill (top half sphere, cylinder, bottom half sphere)
    const auto h = glm::length(_srcGeom->p1 - _srcGeom->p0);
    const auto totalLen = _srcGeom->r0 * 2.f + h;
    const auto sampledLen = totalLen * u[0];

    if(sampledLen <= _srcGeom->r0) // We are in top sphere
    {
        const auto bDir = glm::normalize(_srcGeom->p0 - _srcGeom->p1);
        const pbrt::Vector3f pDir = pbrt::Normalize(pbrt::Vector3f (bDir.x, bDir.y, bDir.z));

        const auto sample = pbrt::UniformSampleHemisphere(u);
        auto sampleDir = pbrt::Vector3f(_srcGeom->p0.x, _srcGeom->p0.y, _srcGeom->p0.z) + _srcGeom->r0 * sample;

        if(pbrt::Dot(pDir, pbrt::Normalize(sampleDir)) < 0.f)
            sampleDir = -sampleDir;

        i.p = pbrt::Point3f(sampleDir.x, sampleDir.y, sampleDir.z);
        i.n = pbrt::Normal3f(pbrt::Normalize(sampleDir));
    }
    else if (sampledLen > _srcGeom->r0 && sampledLen <= _srcGeom->r0 + h) // In frustrum cone
    {
        // Find a vector perpendicular to the pill main direction
        const auto bDir = glm::normalize(_srcGeom->p1 - _srcGeom->p0);
        auto v1 = 1.f, v2 = 1.f;
        auto v3 = (-bDir.x - bDir.y) / (bDir.z);
        const auto ortho = glm::normalize(glm::vec3(v1, v2, v3));

        // Rotate it based on the given sample
        const auto phi = u[1] * 2.f * PBRT_M_PI;
        auto rotated = glm::rotate(ortho, static_cast<float>(phi), bDir);

        // Adjust radius based on height
        auto normaHeight = (sampledLen - _srcGeom->r0) / h;
        //const auto rad = glm::lerp(_srcGeom->r0, _srcGeom->r1, normaHeight);
        const auto rad = pbrt::Lerp(normaHeight, _srcGeom->r0, _srcGeom->r1);
        auto sample = _srcGeom->p0 + rotated * rad;

        // Adjust position along the fustrum cone
        sample += (bDir * normaHeight);

        i.p = pbrt::Point3f(rotated.x, rotated.y, rotated.z);
        i.n = pbrt::Normalize(pbrt::Normal3f(rotated.x, rotated.y, rotated.z));
    }
    else // In bottom sphere
    {
        const auto bDir = glm::normalize(_srcGeom->p1 - _srcGeom->p0);
        const pbrt::Vector3f pDir = pbrt::Normalize(pbrt::Vector3f (bDir.x, bDir.y, bDir.z));

        const auto sample = pbrt::UniformSampleHemisphere(u);
        auto sampleDir = pbrt::Vector3f(_srcGeom->p1.x, _srcGeom->p1.y, _srcGeom->p1.z) + _srcGeom->r0 * sample;

        if(pbrt::Dot(pDir, pbrt::Normalize(sampleDir)) < 0.f)
            sampleDir = -sampleDir;

        i.p = pbrt::Point3f(sampleDir.x, sampleDir.y, sampleDir.z);
        i.n = pbrt::Normal3f(pbrt::Normalize(sampleDir));
    }

    *pdf = 1.f / Area();

    return i;
}
}

#endif
