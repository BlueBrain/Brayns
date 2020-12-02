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
#include "Util.h"

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
inline pbrt::Float reduceMax(const pbrt::Point3f& v, const pbrt::Float t)
{
    return std::max(std::max(v.x, v.y), std::max(v.z, t));
}

inline pbrt::Float reduceMax(const pbrt::Point3f& v)
{
    return std::max(std::max(v.x, v.y), v.z);
}

template<typename T>
inline T abs(const T& v)
{
    return T(fabs(static_cast<double>(v.x)),
             fabs(static_cast<double>(v.y)),
             fabs(static_cast<double>(v.z)));
}

inline pbrt::Float calcEpsilon(const pbrt::Point3f& p, const pbrt::Float t)
{
    return reduceMax(abs(p), t) * PBRT_ULP_EPSILON;
}

inline pbrt::Float smoothstep(const pbrt::Float x)
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

inline pbrt::Float sdSphere(const pbrt::Point3f& p, const pbrt::Point3f& c, pbrt::Float r)
{
    return static_cast<pbrt::Float>((p - c).Length() - r);
}

inline pbrt::Float sdCapsule(const pbrt::Point3f& p, const pbrt::Point3f& a, const pbrt::Point3f& b, pbrt::Float r)
{
    const pbrt::Vector3f pa = p - a, ba = b - a;
    const pbrt::Float h = pbrt::Clamp(pbrt::Dot(pa, ba) / pbrt::Dot(ba, ba), pbrt::Float(0), pbrt::Float(1));
    return (pa - ba * h).Length() - r;
}

inline pbrt::Float sdConePill(const pbrt::Point3f& p, const pbrt::Point3f& p0, const pbrt::Point3f& p1,
                        const pbrt::Float r0, const pbrt::Float r1, const bool useSigmoid)
{
    const pbrt::Vector3f v = (p1 - p0);
    const pbrt::Vector3f w = (p - p0);

    // distance to p0 along cone axis
    const pbrt::Float c1 = pbrt::Dot(w, v);
    if (c1 <= 0)
    {
        return (p - p0).Length() - r0;
    }

    // cone length
    const pbrt::Float c2 = pbrt::Dot(v, v);
    if (c2 <= c1)
    {
        return (p - p1).Length() - r1;
    }

    const pbrt::Float b = c1 / c2;
    const pbrt::Point3f Pb = p0 + b * v;

    const pbrt::Float thickness = useSigmoid
                        ? pbrt::Lerp(smootherstep(b), r0, r1)
                        : pbrt::Lerp(b, r0, r1);

    return (p - Pb).Length() - thickness;
}

inline pbrt::Float calcDistance(const SDFGeometry& primitive, const pbrt::Point3f& p)
{
    switch(primitive.type)
    {
    case SDFType::Sphere:
        return sdSphere(p, TO_PBRT_P3(primitive.p0), primitive.r0);
    case SDFType::Pill:
        return sdCapsule(p, TO_PBRT_P3(primitive.p0), TO_PBRT_P3(primitive.p1), primitive.r0);
    case SDFType::ConePill:
        return sdConePill(p, TO_PBRT_P3(primitive.p0), TO_PBRT_P3(primitive.p1), primitive.r0,
                          primitive.r1, false);
    case SDFType::ConePillSigmoid:
        return sdConePill(p, TO_PBRT_P3(primitive.p0), TO_PBRT_P3(primitive.p1), primitive.r0,
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
/*
    pbrt::Bounds3f WorldBound() const final
    {
      return *ObjectToWorld(_bounds);
    }
*/
    bool Intersect(const pbrt::Ray& r, pbrt::Float* tHit, pbrt::SurfaceInteraction* isect,
                   bool testAlphaTexture = true) const final
    {
      (void)testAlphaTexture;
      pbrt::Vector3f oErr, dErr;
      pbrt::Ray ray = (*WorldToObject)(r, &oErr, &dErr);

      *tHit = raymarch(ray);
      if(*tHit > 0.f && *tHit < ray.tMax)
      {
          const pbrt::Point3f bPos = ray.o + (*tHit * pbrt::Normalize(ray.d));
          const pbrt::Normal3f bNormal = computeNormal(bPos, calcEpsilon(ray.o, *tHit));
          const pbrt::Point3f pHit = ray(*tHit);
          const pbrt::Vector3f pError = pbrt::gamma(5) * pbrt::Abs(static_cast<pbrt::Vector3f>(pHit));

          pbrt::Vector3f dpdu, dpdv;
          pbrt::Normal3f dndu, dndv;
          pbrt::Point2f uv;

          dndu = dndv = pbrt::Normal3f(0.f, 0.f, 0.f);
          uv = pbrt::Point2f(0.f, 0.f);
          pbrt::CoordinateSystem(pbrt::Vector3f(bNormal.x, bNormal.y, bNormal.z), &dpdu, &dpdv);

          *isect = pbrt::SurfaceInteraction(pHit, pError, uv, -ray.d, dpdu, dpdv,
                                            dndu, dndv, ray.time, this);
        
          isect->n = isect->shading.n = bNormal;

          return true;
      }

      return false;
    }

    bool IntersectP(const pbrt::Ray& r, bool testAlphaTexture = true) const final
    {
        (void)testAlphaTexture;
        pbrt::Vector3f oErr, dErr;
        const pbrt::Ray ray = (*WorldToObject)(r, &oErr, &dErr);
        const pbrt::Float t = raymarch(ray);
        return t > 0.f && t < ray.tMax;
    }

    pbrt::Float Area() const final;
    pbrt::Interaction Sample(const pbrt::Point2f& u, pbrt::Float* pdf) const final;

    const SDFGeometry& getSDFGeometry() const { return *_srcGeom; }
    const SDFGeometryData& getSDFGeometryData() const { return *_srcData; }

private:
    pbrt::Float shapeDistance(const pbrt::Point3f& p) const;

    pbrt::Float sdfDistance(const pbrt::Point3f& p) const
    {
        pbrt::Float d = shapeDistance(p);

        // TODO don't blend soma if far enough from eye
        const auto nindx = _srcGeom->neighboursIndex;
        const auto& neighbors = _srcData->neighbours[nindx];
        const auto numNeigh = neighbors.size();

        if(numNeigh > 0)
        {
          const pbrt::Float r0 = _srcGeom->r0;

          for (uint32_t i = 0; i < numNeigh; ++i)
          {
              const SDFGeometry& neighbour = _srcData->geometries[neighbors[i]];

              const pbrt::Float dOther = calcDistance(neighbour, p);
              const pbrt::Float r1 = neighbour.r0;
              const pbrt::Float blendFactor = pbrt::Lerp(pbrt::Float(PBRT_SDF_BLEND_LERP_FACTOR), std::min(r0, r1), std::max(r0, r1));

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
    pbrt::Normal3f computeNormal(const pbrt::Point3f& pos, const pbrt::Float e) const
    {
        // tetrahedron technique (4 evaluations)
        static const pbrt::Vector3f k0 (1, -1, -1);
        static const pbrt::Vector3f k1 (-1, -1, 1);
        static const pbrt::Vector3f k2 (-1, 1, -1);
        static const pbrt::Vector3f k3 (1, 1, 1);

        pbrt::Vector3f temp = pbrt::Normalize(k0 * sdfDistance(pos + e * k0) +
                                              k1 * sdfDistance(pos + e * k1) +
                                              k2 * sdfDistance(pos + e * k2) +
                                              k3 * sdfDistance(pos + e * k3));

        // Fix it due precission problems (TODO check when PBRT_FLOAT is double precission) 
        pbrt::Float test = pbrt::Dot(temp, temp);
        pbrt::Float tempE = e;
        uint32_t i = 0;
        while(glm::isnan(test) && i < PBRT_MAX_NORMAL_FIX_ITERATIONS)
        {
          tempE *= 10.f;
          temp = pbrt::Normalize(k0 * sdfDistance(pos + tempE * k0) +
                                 k1 * sdfDistance(pos + tempE * k1) +
                                 k2 * sdfDistance(pos + tempE * k2) +
                                 k3 * sdfDistance(pos + tempE * k3));
          test = pbrt::Dot(temp, temp);
          ++i;
        }

        if(pbrt::isNaN(std::abs(test)))
          throw std::runtime_error("PBRTSDFGeometryShape: Tetrahedron evaluation for normal failed");

        return pbrt::Normal3f(temp.x, temp.y, temp.z);
    }

    /**
     * Raymarch algorithm
     */
    pbrt::Float raymarch(const pbrt::Ray& ray) const
    {
        pbrt::Float bbHit0 = 0.f, bbHit1 = 0.f;
        if(!_bounds.IntersectP(ray, &bbHit0, &bbHit1))
            return -1.f;

        const pbrt::Float initialDistance = sdfDistance(ray.o);
        const pbrt::Float sdfSign = initialDistance < 0.f? -1.f : 1.f;

        pbrt::Float t = bbHit0;
        const pbrt::Float stepLength = (bbHit1 - bbHit0) / 
                                       static_cast<pbrt::Float>(PBRT_MAX_MARCHING_ITERATIONS);

        for (int i = 0; i < PBRT_MAX_MARCHING_ITERATIONS; ++i)
        {
            // Current position to evaluate
            const pbrt::Float currentT = bbHit0 + stepLength * static_cast<pbrt::Float>(i + 1);
            const pbrt::Point3f p = ray.o + ray.d * currentT;

            // Current distance to closest point of the shape
            pbrt::Float currentDist = sdfDistance(p);
            pbrt::Float currentSign = currentDist < 0.f? -1.f : 1.f;

            // We cross the surface, refine hit and return
            if(currentSign != sdfSign)
            {
                pbrt::Float start = t;
                pbrt::Float end = currentT;

                for (int j = 0; j < PBRT_MAX_MARCH_FIX_ITERATIONS; ++j)
                {
                    pbrt::Float midPoint = (end + start) * pbrt::Float(0.5);
                    pbrt::Point3f pFix = ray.o + ray.d * midPoint;
                    currentDist = sdfDistance(pFix);
                    currentSign = currentDist < 0.? -1. : 1.;

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
        return -1.;
    }

private:
    const SDFGeometry* const _srcGeom;
    const SDFGeometryData* const _srcData;
    pbrt::Bounds3f _bounds;
};


template<>
pbrt::Float PBRTSDFGeometryShape<SDFType::Sphere>::shapeDistance(const pbrt::Point3f& p) const
{
    return sdSphere(p, TO_PBRT_P3(_srcGeom->p0), _srcGeom->r0);
}

template<>
pbrt::Float PBRTSDFGeometryShape<SDFType::Pill>::shapeDistance(const pbrt::Point3f& p) const
{
    return sdCapsule(p, TO_PBRT_P3(_srcGeom->p0), TO_PBRT_P3(_srcGeom->p1), _srcGeom->r0);
}

template<>
pbrt::Float PBRTSDFGeometryShape<SDFType::ConePill>::shapeDistance(const pbrt::Point3f& p) const
{
    return sdConePill(p, TO_PBRT_P3(_srcGeom->p0), TO_PBRT_P3(_srcGeom->p1),
                      _srcGeom->r0, _srcGeom->r1, false);
}

template<>
pbrt::Float PBRTSDFGeometryShape<SDFType::ConePillSigmoid>::shapeDistance(const pbrt::Point3f& p) const
{
    return sdConePill(p, TO_PBRT_P3(_srcGeom->p0), TO_PBRT_P3(_srcGeom->p1),
                      _srcGeom->r0, _srcGeom->r1, true);
}

template<>
pbrt::Float PBRTSDFGeometryShape<SDFType::Sphere>::Area() const
{
    return PBRT_M_PI * pbrt::Float(4) * _srcGeom->r0 * _srcGeom->r0;
}

template<>
pbrt::Float PBRTSDFGeometryShape<SDFType::Pill>::Area() const
{
    const auto h = (TO_PBRT_P3(_srcGeom->p1) - TO_PBRT_P3(_srcGeom->p0)).Length();
    return PBRT_M_PI * pbrt::Float(2) * _srcGeom->r0 * (_srcGeom->r0 * pbrt::Float(2) + h);
}

template<>
pbrt::Float PBRTSDFGeometryShape<SDFType::ConePill>::Area() const
{
    const auto h = (TO_PBRT_P3(_srcGeom->p1) - TO_PBRT_P3(_srcGeom->p0)).Length();
    const auto s = sqrt((_srcGeom->r1 - _srcGeom->r0)*(_srcGeom->r1 - _srcGeom->r0) + h*h);

    return PBRT_M_PI * (_srcGeom->r0 + _srcGeom->r1) * (2. + s);
}

template<>
pbrt::Float PBRTSDFGeometryShape<SDFType::ConePillSigmoid>::Area() const
{
    const auto h = (TO_PBRT_P3(_srcGeom->p1) - TO_PBRT_P3(_srcGeom->p0)).Length();
    const auto s = sqrt((_srcGeom->r1 - _srcGeom->r0)*(_srcGeom->r1 - _srcGeom->r0) + h*h);

    return PBRT_M_PI * (_srcGeom->r0 + _srcGeom->r1) * (2. + s);
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

    Boxd bb = getSDFBoundingBox(*_srcGeom);
    const auto bbCenter = bb.getCenter();
    const auto radius = glm::length(bb.getMax() - bb.getMin()) * 0.5;
    const auto sampleVector = pbrt::UniformSampleSphere(u);
    const auto sample = radius * sampleVector;
    const pbrt::Point3f rayO = pbrt::Point3f(bbCenter.x + sample.x,
                                    bbCenter.y + sample.y,
                                    bbCenter.z + sample.z);
    const pbrt::Vector3f rayD = pbrt::Normalize(-sampleVector);

    const pbrt::Float initialDistance = shapeDistance(rayO);
    const pbrt::Float sdfSign = initialDistance < 0.f? -1.f : 1.f;

    pbrt::Float t = 0;
    const pbrt::Float stepLength = (radius * 2.0) /
            static_cast<pbrt::Float>(PBRT_MAX_MARCHING_ITERATIONS);

    for (int i = 0; i < PBRT_MAX_MARCHING_ITERATIONS; ++i)
    {
        // Current position to evaluate
        const auto currentT = 0 + stepLength * static_cast<pbrt::Float>(i + 1);
        const pbrt::Point3f p = rayO + rayD * currentT;

        // Current distance to closest point of the shape
        pbrt::Float currentDist = shapeDistance(p);
        pbrt::Float currentSign = currentDist < 0.f? -1.f : 1.f;

        // We cross the surface, refine hit and return
        if(currentSign != sdfSign)
        {
            auto start = t;
            auto end = currentT;

            for (int j = 0; j < PBRT_MAX_MARCH_FIX_ITERATIONS; ++j)
            {
                auto midPoint = (end + start) * 0.5f;
                auto pFix = rayO + rayD * midPoint;
                currentDist = shapeDistance(pFix);
                currentSign = currentDist < 0.f? -1.f : 1.f;

                if(currentSign != sdfSign)
                    end = midPoint;
                else
                    start = midPoint;
            }

            t = start;
            break;
        }

        t = currentT;
    }

    *pdf = 1.f / Area();
    i.p = rayO + rayD * t;
    i.n = computeNormal(i.p, calcEpsilon(rayO, t));

    return i;
}

template<>
pbrt::Interaction 
PBRTSDFGeometryShape<SDFType::ConePill>::Sample(const pbrt::Point2f& u, 
                                                pbrt::Float* pdf) const
{
    pbrt::Interaction i;

    Boxd bb = getSDFBoundingBox(*_srcGeom);
    const auto bbCenter = bb.getCenter();
    const auto radius = glm::length(bb.getMax() - bb.getMin()) * 0.5;
    const auto sampleVector = pbrt::UniformSampleSphere(u);
    const auto sample = radius * sampleVector;
    const pbrt::Point3f rayO = pbrt::Point3f(bbCenter.x + sample.x,
                                    bbCenter.y + sample.y,
                                    bbCenter.z + sample.z);
    const pbrt::Vector3f rayD = pbrt::Normalize(-sampleVector);

    const pbrt::Float initialDistance = shapeDistance(rayO);
    const pbrt::Float sdfSign = initialDistance < 0.f? -1.f : 1.f;

    pbrt::Float t = 0;
    const pbrt::Float stepLength = (radius * 2.0) /
            static_cast<pbrt::Float>(PBRT_MAX_MARCHING_ITERATIONS);

    for (int i = 0; i < PBRT_MAX_MARCHING_ITERATIONS; ++i)
    {
        // Current position to evaluate
        const auto currentT = 0 + stepLength * static_cast<pbrt::Float>(i + 1);
        const pbrt::Point3f p = rayO + rayD * currentT;

        // Current distance to closest point of the shape
        pbrt::Float currentDist = shapeDistance(p);
        pbrt::Float currentSign = currentDist < 0.f? -1.f : 1.f;

        // We cross the surface, refine hit and return
        if(currentSign != sdfSign)
        {
            auto start = t;
            auto end = currentT;

            for (int j = 0; j < PBRT_MAX_MARCH_FIX_ITERATIONS; ++j)
            {
                auto midPoint = (end + start) * 0.5f;
                auto pFix = rayO + rayD * midPoint;
                currentDist = shapeDistance(pFix);
                currentSign = currentDist < 0.f? -1.f : 1.f;

                if(currentSign != sdfSign)
                    end = midPoint;
                else
                    start = midPoint;
            }

            t = start;
            break;
        }

        t = currentT;
    }

    *pdf = 1.f / Area();
    i.p = rayO + rayD * t;
    i.n = computeNormal(i.p, calcEpsilon(rayO, t));

    return i;
}

template<>
pbrt::Interaction 
PBRTSDFGeometryShape<SDFType::ConePillSigmoid>::Sample(const pbrt::Point2f& u, 
                                                       pbrt::Float* pdf) const
{
    pbrt::Interaction i;

    Boxd bb = getSDFBoundingBox(*_srcGeom);
    const auto bbCenter = bb.getCenter();
    const auto radius = glm::length(bb.getMax() - bb.getMin()) * 0.5;
    const auto sampleVector = pbrt::UniformSampleSphere(u);
    const auto sample = radius * sampleVector;
    const pbrt::Point3f rayO = pbrt::Point3f(bbCenter.x + sample.x,
                                    bbCenter.y + sample.y,
                                    bbCenter.z + sample.z);
    const pbrt::Vector3f rayD = pbrt::Normalize(-sampleVector);

    const pbrt::Float initialDistance = shapeDistance(rayO);
    const pbrt::Float sdfSign = initialDistance < 0.f? -1.f : 1.f;

    pbrt::Float t = 0;
    const pbrt::Float stepLength = (radius * 2.0) /
            static_cast<pbrt::Float>(PBRT_MAX_MARCHING_ITERATIONS);

    for (int i = 0; i < PBRT_MAX_MARCHING_ITERATIONS; ++i)
    {
        // Current position to evaluate
        const auto currentT = 0 + stepLength * static_cast<pbrt::Float>(i + 1);
        const pbrt::Point3f p = rayO + rayD * currentT;

        // Current distance to closest point of the shape
        pbrt::Float currentDist = shapeDistance(p);
        pbrt::Float currentSign = currentDist < 0.f? -1.f : 1.f;

        // We cross the surface, refine hit and return
        if(currentSign != sdfSign)
        {
            auto start = t;
            auto end = currentT;

            for (int j = 0; j < PBRT_MAX_MARCH_FIX_ITERATIONS; ++j)
            {
                auto midPoint = (end + start) * 0.5f;
                auto pFix = rayO + rayD * midPoint;
                currentDist = shapeDistance(pFix);
                currentSign = currentDist < 0.f? -1.f : 1.f;

                if(currentSign != sdfSign)
                    end = midPoint;
                else
                    start = midPoint;
            }

            t = start;
            break;
        }

        t = currentT;
    }

    *pdf = 1.f / Area();
    i.p = rayO + rayD * t;
    i.n = computeNormal(i.p, calcEpsilon(rayO, t));

    return i;
}
}

#endif
