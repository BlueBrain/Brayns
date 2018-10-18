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

#define SDF_TYPE_SPHERE 0
#define SDF_TYPE_PILL 1
#define SDF_TYPE_CONE_PILL 2
#define SDF_TYPE_CONE_PILL_SIGMOID 3

#define SDF_BLEND_FACTOR 0.1
#define SDF_BLEND_LERP_FACTOR 0.2

#define SDF_EPSILON 0.000001

/////////////////////////////////////////////////////////////////////////////

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
    const float h = clamp(dot(pa, ba) / dot(ba, ba), 0.0, 1.0);
    return length(pa - ba * h) - r;
}

inline float mix(const float x, const float y, const float a)
{
    return x * (1.f - a) + y * a;
}

// polynomial smooth min (k = 0.1);
inline float sminPoly(const float a, const float b, const float k)
{
    const float h = clamp(0.5 + 0.5 * (b - a) / k, 0.0, 1.0);
    return mix(b, a, h) - k * h * (1.0 - h);
}

inline float sdConePill(const vec3f& p, const vec3f p0, const vec3f p1,
                        const float radius_bottom, const float radius_top,
                        const bool useSigmoid)
{
    const vec3f v = p1 - p0;
    const vec3f w = p - p0;

    const float c1 = dot(w, v);
    if (c1 <= 0)
        return length(p - p0) - radius_bottom;

    const float c2 = dot(v, v);
    if (c2 <= c1)
        return length(p - p1) - radius_top;

    const float b = c1 / c2;
    const vec3f Pb = p0 + b * v;

    const float thicknessAt = mix(radius_bottom, radius_top, b);
    const float thickness =
        useSigmoid
            ? 0.5 * (thicknessAt +
                     mix(radius_bottom, radius_top, smootherstep(b)))
            : thicknessAt;

    return length(p - Pb) - thickness;
}

// AABB-Ray intersection ( http://prideout.net/blog/?p=64 ).
inline varying bool intersectBox(Ray& ray, const vec3f aabbMin,
                                 const vec3f aabbMax, float& t0, float& t1)
{
    const float epsilon = SDF_EPSILON;

    Ray r = ray;
    // We need to avoid division by zero in "vec3 invR = 1.0 / r.Dir;"
    if (r.dir.x == 0)
        r.dir.x = epsilon;

    if (r.dir.y == 0)
        r.dir.y = epsilon;

    if (r.dir.z == 0)
        r.dir.z = epsilon;

    const vec3f invR = 1.f / r.dir;
    const vec3f tbot = invR * (aabbMin - r.org);
    const vec3f ttop = invR * (aabbMax - r.org);
    const vec3f tmin = min(ttop, tbot);
    const vec3f tmax = max(ttop, tbot);
    vec2f t = make_vec2f(max(tmin.x, tmin.y), max(tmin.x, tmin.z));
    t0 = max(t.x, t.y);
    t = make_vec2f(min(tmax.x, tmax.y), min(tmax.x, tmax.z));
    t1 = min(t.x, t.y);
    return (t0 <= t1);
}

/////////////////////////////////////////////////////////////////////////////

// NOTE: This layout must match exactly the 'SDFGeometry' struct in
// 'SDFGeometry.h'
struct SDFGeometry
{
    uint64 userData;
    vec3f center;
    vec3f p0;
    vec3f p1;
    float radius;
    float radius_tip;
    uint64 neighboursIndex;
    uint8 numNeighbours;
    uint8 type;
};

struct SDFGeometries
{
    uniform Geometry super;

    uniform uint64* uniform geometryRefs;
    uniform uint64* uniform neighbours;
    uniform SDFGeometry* uniform geometries;

    uniform bool useSafeIncrement;
};

DEFINE_SAFE_INCREMENT(SDFGeometry);
DEFINE_SAFE_INCREMENT(uint64);

/////////////////////////////////////////////////////////////////////////////

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

const uniform SDFGeometry* uniform getPrimitive(
    const uniform SDFGeometries& geometry, const uniform uint64 idx)
{
    return safeIncrement(geometry.useSafeIncrement, geometry.geometries, idx);
}

const uniform SDFGeometry* varying getPrimitiveVarying(
    const uniform SDFGeometries& geometry, const varying uint64 idx)
{
    return safeIncrement(geometry.useSafeIncrement, geometry.geometries, idx);
}

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

uniform box3fa calcBounds(const uniform SDFGeometry& primitive)
{
    if (primitive.type == SDF_TYPE_SPHERE)
    {
        return make_box3fa(primitive.center - make_vec3f(primitive.radius),
                           primitive.center + make_vec3f(primitive.radius));
    }
    // else primitive.type == SDF_TYPE_CONE_PILL ||
    //      primitive.type == SDF_TYPE_PILL ||
    //      primitive.type == SDF_TYPE_CONE_PILL_SIGMOID
    const uniform vec3f minV = make_vec3f(min(primitive.p0.x, primitive.p1.x),
                                          min(primitive.p0.y, primitive.p1.y),
                                          min(primitive.p0.z, primitive.p1.z));

    const uniform vec3f maxV = make_vec3f(max(primitive.p0.x, primitive.p1.x),
                                          max(primitive.p0.y, primitive.p1.y),
                                          max(primitive.p0.z, primitive.p1.z));
    return make_box3fa(minV - make_vec3f(primitive.radius),
                       maxV + make_vec3f(primitive.radius));
}

void SDFGeometries_bounds(const RTCBoundsFunctionArguments* uniform args)
{
    const uniform SDFGeometries* uniform self =
        (const uniform SDFGeometries* uniform)args->geometryUserPtr;
    const uniform uint64 primID = args->primID;

    const uniform uint64 idx = primToIdx(*self, primID);

    *((box3fa * uniform)args->bounds_o) = calcBounds(*getPrimitive(*self, idx));
}

inline float calcDistance(const uniform SDFGeometry& primitive, const vec3f& p)
{
    if (primitive.type == SDF_TYPE_SPHERE)
        return sdSphere(p, primitive.center, primitive.radius);
    if (primitive.type == SDF_TYPE_PILL)
        return sdCapsule(p, primitive.p0, primitive.p1, primitive.radius);
    if (primitive.type == SDF_TYPE_CONE_PILL ||
        primitive.type == SDF_TYPE_CONE_PILL_SIGMOID)
    {
        return sdConePill(p, primitive.p0, primitive.p1, primitive.radius,
                          primitive.radius_tip,
                          primitive.type == SDF_TYPE_CONE_PILL_SIGMOID);
    }

    return -1.0;
}

inline float map(const uniform SDFGeometry& primitive, const vec3f& p,
                 const uniform SDFGeometries& geometry)
{
    float d = calcDistance(primitive, p);
    const float r0 = primitive.radius;

    const uint8 numNeighs = primitive.numNeighbours;

    for (uniform int i = 0; i < numNeighs; i++)
    {
        const uniform uint64 index =
            getNeighbourIdx(geometry, primitive.neighboursIndex, i);

        const uniform SDFGeometry& neighbour = *getPrimitive(geometry, index);

        const float dOther = calcDistance(neighbour, p);
        const float r1 = neighbour.radius;
        const float blendFactor =
            mix(min(r0, r1), max(r0, r1), SDF_BLEND_LERP_FACTOR);

        d = sminPoly(dOther, d, blendFactor * SDF_BLEND_FACTOR);
    }

    return d;
}

inline vec3f calcNormal(const uniform SDFGeometry& primitive, const float eps,
                        const vec3f& pos, const uniform SDFGeometries& geometry)
{
    const float x0 = map(primitive, pos + make_vec3f(eps, 0.f, 0.f), geometry);
    const float x1 = map(primitive, pos - make_vec3f(eps, 0.f, 0.f), geometry);
    const float y0 = map(primitive, pos + make_vec3f(0.f, eps, 0.f), geometry);
    const float y1 = map(primitive, pos - make_vec3f(0.f, eps, 0.f), geometry);
    const float z0 = map(primitive, pos + make_vec3f(0.f, 0.f, eps), geometry);
    const float z1 = map(primitive, pos - make_vec3f(0.f, 0.f, eps), geometry);

    return normalize(make_vec3f(x0 - x1, y0 - y1, z0 - z1));
}

inline float intersect(const uniform SDFGeometry& primitive, const vec3f& ro,
                       const vec3f& rd, const uniform SDFGeometries& geometry)
{
    Ray ray;
    ray.dir = rd;
    ray.org = ro;

    const uniform box3fa bbox = calcBounds(primitive);

    float t0_pre, t1_pre;
    const vec3f aabbMin = make_vec3f(bbox.lower);
    const vec3f aabbMax = make_vec3f(bbox.upper);

    if (!intersectBox(ray, aabbMin, aabbMax, t0_pre, t1_pre))
        return -1;

    const float t0 = t0_pre;
    const float t1 = t1_pre;

    const int samplesPerRay = 100;
    float t = t0;
    float res = -1.f;
    float h = 1.f;
    for (int i = 0; i < samplesPerRay; ++i)
    {
        if (h < SDF_EPSILON || t > t1)
            break;
        h = map(primitive, ro + rd * t, geometry);
        res = t;
        t += h;
    }
    if (t > t1)
        res = -1.f;
    return res;
}

unmasked void SDFGeometries_intersect(
    const RTCIntersectFunctionNArguments* uniform args)
{
    const uniform SDFGeometries* uniform self =
        (uniform SDFGeometries * uniform)args->geometryUserPtr;
    const uniform int primID = args->primID;

    const uniform SDFGeometry& primitive =
        *getPrimitive(*self, primToIdx(*self, primID));

    varying Ray* uniform ray = (varying Ray * uniform)args->rayhit;

    const float t_in = intersect(primitive, ray->org, ray->dir, *self);
    if (t_in > 0 && t_in > ray->t0 && t_in < ray->t)
    {
        const vec3f pos = ray->org + t_in * ray->dir;

        ray->primID = primID;
        ray->geomID = self->super.geomID;
        ray->instID = args->context->instID[0];
        ray->t = t_in;
        ray->Ng = calcNormal(primitive, getIntersectionError(ray->org, t_in),
                             pos, *self);
    }
}

export void* uniform SDFGeometries_create(void* uniform cppEquivalent)
{
    uniform SDFGeometries* uniform geom = uniform new uniform SDFGeometries;
    Geometry_Constructor(&geom->super, cppEquivalent,
                         SDFGeometries_postIntersect, 0, 0, 0);
    return geom;
}

export void SDFGeometriesGeometry_set(void* uniform _self, void* uniform _model,
                                      void* uniform data,
                                      int uniform numPrimitives,
                                      void* uniform neighbours,
                                      int uniform numNeighbours,
                                      void* uniform geometries)
{
    uniform SDFGeometries* uniform self =
        (uniform SDFGeometries * uniform)_self;
    uniform Model* uniform model = (uniform Model * uniform)_model;

    RTCGeometry geom =
        rtcNewGeometry(ispc_embreeDevice(), RTC_GEOMETRY_TYPE_USER);
    uniform uint32 geomID = rtcAttachGeometry(model->embreeSceneHandle, geom);

    self->super.model = model;
    self->super.geomID = geomID;
    self->super.numPrimitives = numPrimitives;
    self->geometryRefs = (uniform uint64 * uniform)data;
    self->neighbours = (uniform uint64 * uniform)neighbours;
    self->geometries = (uniform SDFGeometry * uniform)geometries;

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
