/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Jonas Karlsson <jonas.karlsson@epfl.ch>
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

// ospray
#include "ospray/SDK/common/Model.ih"
#include "ospray/SDK/common/Ray.ih"
#include "ospray/SDK/geometry/Geometry.ih"
#include "ospray/SDK/math/box.ih"
#include "ospray/SDK/math/vec.ih"
#include "ospray/version.h"

// embree
#include "embree3/rtcore.isph"
#include "embree3/rtcore_geometry.isph"
#include "embree3/rtcore_scene.isph"
#include "utils/SafeIncrement.ih"

#include "RayMarching.isph"

#define SDF_TYPE_SPHERE 0
#define SDF_TYPE_PILL 1
#define SDF_TYPE_CONE_PILL 2
#define SDF_TYPE_CONE_PILL_SIGMOID 3

#define SDF_BLEND_FACTOR 0.1
#define SDF_BLEND_LERP_FACTOR 0.2

/////////////////////////////////////////////////////////////////////////////

// https://en.wikipedia.org/wiki/Smoothstep
inline float smoothstep(const float x)
{
    return x * x * (3 - 2 * x);
}

// https://en.wikipedia.org/wiki/Smoothstep
inline float smootherstep(const float x)
{
    return x * x * x * (x * (x * 6 - 15) + 10);
}

inline float sdSphere(const vec3f& p, const vec3f c, float r)
{
    return length(p - c) - r;
}

inline float sdCapsule(const vec3f& p, const vec3f a, const vec3f b, float r)
{
    const vec3f pa = p - a, ba = b - a;
    const float h = clamp(dot(pa, ba) / dot(ba, ba), 0.f, 1.f);
    return length(pa - ba * h) - r;
}

// polynomial smooth min (k = 0.1);
inline float sminPoly(const float a, const float b, const float k)
{
    const float h = clamp(0.5f + 0.5f * (b - a) / k, 0.f, 1.f);
    return lerp(h, b, a) - k * h * (1.f - h);
}

inline float sdConePill(const vec3f& p, const vec3f p0, const vec3f p1,
                        const float r0, const float r1, const bool useSigmoid)
{
    const vec3f v = p1 - p0;
    const vec3f w = p - p0;

    // distance to p0 along cone axis
    const float c1 = dot(w, v);
    if (c1 <= 0)
        return length(p - p0) - r0;

    // cone length
    const float c2 = dot(v, v);
    if (c2 <= c1)
        return length(p - p1) - r1;

    const float b = c1 / c2;
    const vec3f Pb = p0 + b * v;

    const float thicknessAt = lerp(b, r0, r1);
    const float thickness =
        useSigmoid ? lerp(smootherstep(b), r0, r1) : thicknessAt;

    return length(p - Pb) - thickness;
}

//////////////////////////////////////////////////////////////////////

// NOTE: This layout must match exactly the 'SDFGeometry' struct in
// 'SDFGeometry.h'
struct SDFGeometry
{
    uint64 userData;
    vec3f p0;
    vec3f p1;
    float r0;
    float r1;
    uint64 neighboursIndex;
    uint8 numNeighbours;
    uint8 type;
};

struct SDFGeometries
{
    uniform Geometry super;

    uniform SDFGeometry* uniform geometries;
    uniform uint64* uniform geometryRefs;
    uniform uint64* uniform neighbours;

    uniform bool useSafeIncrement;
};

//////////////////////////////////////////////////////////////////////

typedef uniform SDFGeometries* uniform Geo_ptr;
typedef uniform SDFGeometry* uniform Prim_ptr;

//////////////////////////////////////////////////////////////////////

DEFINE_SAFE_INCREMENT(SDFGeometry);
DEFINE_SAFE_INCREMENT(uint64);

//////////////////////////////////////////////////////////////////////

uniform uint64 primToIdx(const uniform SDFGeometries& geometry,
                         const uniform uint64 primID)
{
    return *safeIncrement(geometry.useSafeIncrement, geometry.geometryRefs,
                          primID);
}

varying uint64 primToIdxVarying(const uniform SDFGeometries& geometry,
                                const varying uint64 primID)
{
    return *safeIncrement(geometry.useSafeIncrement, geometry.geometryRefs,
                          primID);
}

uniform uint64 getNeighbourIdx(const uniform SDFGeometries& geometry,
                               const uniform uint64 startIdx,
                               const uniform uint64 neighIdx)
{
    return *safeIncrement(geometry.useSafeIncrement, geometry.neighbours,
                          startIdx + neighIdx);
}

const Prim_ptr getPrimitive(const uniform SDFGeometries& geometry,
                            const uniform uint64 idx)
{
    return safeIncrement(geometry.useSafeIncrement, geometry.geometries, idx);
}

const uniform SDFGeometry* varying getPrimitiveVarying(
    const uniform SDFGeometries& geometry, const varying uint64 idx)
{
    return safeIncrement(geometry.useSafeIncrement, geometry.geometries, idx);
}

//////////////////////////////////////////////////////////////////////

inline float calcDistance(const uniform SDFGeometry& primitive, const vec3f& p)
{
    if (primitive.type == SDF_TYPE_SPHERE)
        return sdSphere(p, primitive.p0, primitive.r0);
    if (primitive.type == SDF_TYPE_PILL)
        return sdCapsule(p, primitive.p0, primitive.p1, primitive.r0);
    if (primitive.type == SDF_TYPE_CONE_PILL ||
        primitive.type == SDF_TYPE_CONE_PILL_SIGMOID)
    {
        return sdConePill(p, primitive.p0, primitive.p1, primitive.r0,
                          primitive.r1,
                          primitive.type == SDF_TYPE_CONE_PILL_SIGMOID);
    }

    return -1.0;
}

//////////////////////////////////////////////////////////////////////

uniform box3fa sdfBounds(uDataPtr_t geo_, uDataPtr_t prim_)
{
    const Prim_ptr prim = (const Prim_ptr)prim_;

    if (prim->type == SDF_TYPE_SPHERE)
    {
        return make_box3fa(prim->p0 - prim->r0, prim->p0 + prim->r0);
    }
    // else prim->type == SDF_TYPE_CONE_PILL ||
    //      prim->type == SDF_TYPE_PILL ||
    //      prim->type == SDF_TYPE_CONE_PILL_SIGMOID
    const uniform vec3f minV = min(prim->p0, prim->p1);
    const uniform vec3f maxV = max(prim->p0, prim->p1);

    return make_box3fa(minV - prim->r0, maxV + prim->r0);
}

//////////////////////////////////////////////////////////////////////

float sdfDistance(const vec3f& p, uDataPtr_t geo_, uDataPtr_t prim_,
                  const SDFParams&)
{
    const Geo_ptr geo = (const Geo_ptr)geo_;
    const Prim_ptr prim = (const Prim_ptr)prim_;

    float d = calcDistance(*prim, p);

    // TODO don't blend soma if far enough from eye

    const float r0 = prim->r0;

    for (uniform int i = 0; i < prim->numNeighbours; i++)
    {
        const uniform uint64 index =
            getNeighbourIdx(*geo, prim->neighboursIndex, i);

        const uniform SDFGeometry& neighbour = *getPrimitive(*geo, index);

        const float dOther = calcDistance(neighbour, p);
        const float r1 = neighbour.r0;
        const float blendFactor =
            lerp(SDF_BLEND_LERP_FACTOR, min(r0, r1), max(r0, r1));

        d = sminPoly(dOther, d, blendFactor * SDF_BLEND_FACTOR);
    }

    return d;
}

//////////////////////////////////////////////////////////////////////

void SDFGeometries_bounds(const RTCBoundsFunctionArguments* uniform args)
{
    const Geo_ptr geo = (Geo_ptr)args->geometryUserPtr;
    const uniform uint64 idx = primToIdx(*geo, args->primID);
    const Prim_ptr prim = getPrimitive(*geo, idx);

    *((box3fa * uniform) args->bounds_o) =
        sdfBounds((uDataPtr_t)geo, (uDataPtr_t)prim);
}

//////////////////////////////////////////////////////////////////////

unmasked void SDFGeometries_intersect(
    const RTCIntersectFunctionNArguments* uniform args)
{
    const Geo_ptr geo = (Geo_ptr)args->geometryUserPtr;
    const uniform uint64 idx = primToIdx(*geo, args->primID);
    const Prim_ptr prim = getPrimitive(*geo, idx);

    varying Ray* uniform ray = (varying Ray * uniform) args->rayhit;

    // dummy parameter struct
    SDFParams sdfParams;

    const float t_in =
        raymarching(*ray, sdfDistance, sdfBounds, (uDataPtr_t)geo,
                    (uDataPtr_t)prim, sdfParams);

    if (t_in > 0 && t_in > ray->t0 && t_in < ray->t)
    {
        const vec3f pos = ray->org + t_in * ray->dir;

        ray->primID = args->primID;
        ray->geomID = geo->super.geomID;
        ray->instID = args->context->instID[0];
        ray->t = t_in;

        // TODO epsilon based on pixel radius

        ray->Ng = computeNormal(pos, sdfDistance, calcEpsilon(ray->org, t_in),
                                (uDataPtr_t)geo, (uDataPtr_t)prim, sdfParams);
    }
}

//////////////////////////////////////////////////////////////////////

static void SDFGeometries_postIntersect(uniform Geometry* uniform geometry,
                                        uniform Model* uniform model,
                                        varying DifferentialGeometry& dg,
                                        const varying Ray& ray,
                                        uniform int64 flags)
{
    dg.geometry = geometry;
    vec3f Ng = ray.Ng;
    vec3f Ns = Ng;

    if (flags & DG_NORMALIZE)
    {
        Ng = normalize(Ng);
        Ns = normalize(Ns);
    }
    if (flags & DG_FACEFORWARD)
    {
        if (dot(ray.dir, Ng) >= 0.f)
            Ng = neg(Ng);
        if (dot(ray.dir, Ns) >= 0.f)
            Ns = neg(Ns);
    }

    dg.Ng = Ng;
    dg.Ns = Ns;
}

//////////////////////////////////////////////////////////////////////

export void* uniform SDFGeometries_create(void* uniform cppEquivalent)
{
    Geo_ptr geom = uniform new uniform SDFGeometries;
    Geometry_Constructor(&geom->super, cppEquivalent,
                         SDFGeometries_postIntersect, 0, 0, 0);
    return geom;
}

//////////////////////////////////////////////////////////////////////

export void SDFGeometriesGeometry_set(void* uniform _self, void* uniform _model,
                                      void* uniform data,
                                      int uniform numPrimitives,
                                      void* uniform neighbours,
                                      int uniform numNeighbours,
                                      void* uniform geometries)
{
    Geo_ptr self = (Geo_ptr)_self;
    uniform Model* uniform model = (uniform Model * uniform) _model;

    RTCGeometry geom =
        rtcNewGeometry(ispc_embreeDevice(), RTC_GEOMETRY_TYPE_USER);
    uniform uint32 geomID = rtcAttachGeometry(model->embreeSceneHandle, geom);

    self->super.model = model;
    self->super.geomID = geomID;
    self->super.numPrimitives = numPrimitives;
    self->geometryRefs = (uniform uint64 * uniform) data;
    self->neighbours = (uniform uint64 * uniform) neighbours;
    self->geometries = (uniform SDFGeometry * uniform) geometries;

    // NOTE: self->data is always smaller than self->geometries
    self->useSafeIncrement =
        needsSafeIncrement(self->geometries, numPrimitives) ||
        needsSafeIncrement(self->neighbours, numNeighbours);

    rtcSetGeometryUserData(geom, self);
    rtcSetGeometryUserPrimitiveCount(geom, numPrimitives);
    rtcSetGeometryBoundsFunction(
        geom, (uniform RTCBoundsFunction)&SDFGeometries_bounds, self);
    rtcSetGeometryIntersectFunction(
        geom, (uniform RTCIntersectFunctionN)&SDFGeometries_intersect);
    rtcSetGeometryOccludedFunction(
        geom, (uniform RTCOccludedFunctionN)&SDFGeometries_intersect);
    rtcCommitGeometry(geom);
    rtcReleaseGeometry(geom);
}
