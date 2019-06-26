/* Copyright (c) 2019, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Author: Sebastien Speierer <sebastien.speierer@epfl.ch>
 * *
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
// embree
#include "embree3/rtcore.isph"
#include "embree3/rtcore_geometry.isph"
#include "embree3/rtcore_scene.isph"

#include "ospray/SDK/math/vec.ih"

#include "utils/SafeIncrement.ih"

#include "RayMarching.isph"

//////////////////////////////////////////////////////////////////////

struct SDFBezier
{
    uint64 userData;
    vec3f p0; // starting point
    vec3f c0; // first control point
    float r0; // starting radius
    vec3f p1; // ending point
    vec3f c1; // second control point
    float r1; // ending radius
};

DEFINE_SAFE_INCREMENT(SDFBezier);

//////////////////////////////////////////////////////////////////////

inline uniform float bezier(uniform float t, uniform float p0, uniform float c0,
                            uniform float p1, uniform float c1)
{
    const uniform float s = 1 - t;
    const uniform float s2 = s * s;
    const uniform float t2 = t * t;
    return s * s2 * p0 + 3 * s2 * t * c0 + 3 * s * t2 * c1 + t * t2 * p1;
}

inline vec3f bezier(float t, const uniform vec3f& p0, const uniform vec3f& c0,
                    const uniform vec3f& p1, const uniform vec3f& c1)
{
    const vec3f s = make_vec3f(1 - t);
    const vec3f s2 = s * s;
    const vec3f t2 = make_vec3f(t * t);
    return s * s2 * p0 + 3 * s2 * t * c0 + 3 * s * t2 * c1 + t * t2 * p1;
}

//////////////////////////////////////////////////////////////////////

inline float sdf_tube(const vec3f& P, const vec3f& P0, const vec3f& P1,
                      float r0_, float r1_, float t, float deltaT)
{
#define TANGENTIAL_TUBE
#ifndef TANGENTIAL_TUBE
    const vec3f v = P1 - P0;
    const vec3f w = P - P0;

    // distance to P0 along tube axis
    const float c1 = dot(w, v);
    if (c1 < 0.0f)
        return length(P - P0) - lerp(t - deltaT, r0_, r1_);

    // tube length
    const float c2 = dot(v, v);
    if (c2 <= c1)
        return length(P - P1) - lerp(t, r0_, r1_);

    const float b = c1 / c2;
    vec3f Pb = P0 + b * v;

    const float thickness = lerp(t + (b - 1.0f) * deltaT, r0_, r1_);

    return length(P - Pb) - thickness;
#else
    // based on http://sci.utah.edu/~will/papers/tubes.pdf

    // extremities radii
    const float r0 = lerp(t - deltaT, r0_, r1_);
    const float r1 = lerp(t, r0_, r1_);

    // cone starting and ending points
    vec3f coneP0 = P0;
    vec3f coneP1 = P1;
    // cone starting and ending radii
    float coneR0 = r0;
    float coneR1 = r1;

    // find tangential cone to spheres if needed
    if (abs(r0 - r1) > SDF_EPSILON)
    {
        const vec3f V = P1 - P0;
        const float len = length(V);
        const vec3f C = V / len;

        // distance(P0, A)
        const float d0 = len * r0 / (r1 - r0);
        // distance(P2, A)
        const float d1 = len + d0;
        // apex point
        const vec3f A = P0 - d0 * C;

        // distance (coneP0, A)
        const float z0 = d0 - r0 * r0 / d0;
        // distance (coneP1, A)
        const float z1 = d1 - r1 * r1 / d1;

        // tangential cone starting and ending points
        coneP0 = A + z0 * C;
        coneP1 = A + z1 * C;
        // tangential cone starting and ending radii
        coneR0 = sqrt(sqr(r0) - sqr(d0 - z0));
        coneR1 = sqrt(sqr(r1) - sqr(d1 - z1));
    }

    const vec3f v = coneP1 - coneP0;
    const vec3f w = P - coneP0;

    // distance to P0 along tube axis
    const float c1 = dot(w, v);
    if (c1 <= 0.0f)
        return length(P - P0) - r0;

    // tube length
    const float c2 = dot(v, v);
    if (c2 <= c1)
        return length(P - P1) - r1;

    const float b = c1 / c2;
    vec3f Pb = coneP0 + b * v;

    const float thickness = lerp(b, coneR0, coneR1);

    return length(P - Pb) - thickness;
#endif
}

//////////////////////////////////////////////////////////////////////

struct SDFBeziers
{
    uniform Geometry super;

    uniform SDFBezier* uniform data;

    uniform bool useSafeIncrement;
};

/////////////////////////////////////////////////////////////////////////////

typedef uniform SDFBeziers* uniform Geo_ptr;
typedef uniform SDFBezier* uniform Bezier_ptr;

//////////////////////////////////////////////////////////////////////

uniform box3fa bezierBounds(uDataPtr_t geo_, uDataPtr_t bezier_)
{
#define USE_TIGHT_BOUNDS
#ifndef USE_TIGHT_BOUNDS
    const Bezier_ptr bc = (const Bezier_ptr)bezier_;
    const uniform vec3f mi = min(min(bc->p0 - bc->r0, bc->p1 - bc->r1),
                                 min(bc->c0 - bc->r0, bc->c1 - bc->r1));
    const uniform vec3f ma = max(max(bc->p0 + bc->r0, bc->p1 + bc->r1),
                                 max(bc->c0 + bc->r0, bc->c1 + bc->r1));
    return make_box3fa(mi, ma);
#else
    const Bezier_ptr bc = (const Bezier_ptr)bezier_;

    uniform float r = max(bc->r0, bc->r1);

    // tight bounding box
    uniform vec3f mi = min(bc->p0, bc->p1);
    uniform vec3f ma = max(bc->p0, bc->p1);

    const uniform vec3f c = -1.0f * bc->p0 + bc->c0;
    const uniform vec3f b = 1.0f * bc->p0 - 2 * bc->c0 + bc->c1;
    const uniform vec3f a = -1.0f * bc->p0 + 3 * bc->c0 - 3 * bc->c1 + bc->p1;

    uniform vec3f h = b * b - a * c;

    if (h.x > 0.0f)
    {
        h.x = sqrt(h.x);
        uniform float t = (-b.x - h.x) / a.x;
        if (!isnan(t) && t > 0.0f && t < 1.0f)
        {
            uniform float q = bezier(t, bc->p0.x, bc->c0.x, bc->p1.x, bc->c1.x);
            mi.x = min(mi.x, q);
            ma.x = max(ma.x, q);
        }

        t = (-b.x + h.x) / a.x;
        if (!isnan(t) && t > 0.0f && t < 1.0f)
        {
            uniform float q = bezier(t, bc->p0.x, bc->c0.x, bc->p1.x, bc->c1.x);
            mi.x = min(mi.x, q);
            ma.x = max(ma.x, q);
        }
    }

    if (h.y > 0.0f)
    {
        h.y = sqrt(h.y);
        uniform float t = (-b.y - h.y) / a.y;
        if (!isnan(t) && t > 0.0f && t < 1.0f)
        {
            uniform float q = bezier(t, bc->p0.y, bc->c0.y, bc->p1.y, bc->c1.y);
            mi.y = min(mi.y, q);
            ma.y = max(ma.y, q);
        }
        t = (-b.y + h.y) / a.y;
        if (!isnan(t) && t > 0.0f && t < 1.0f)
        {
            uniform float q = bezier(t, bc->p0.y, bc->c0.y, bc->p1.y, bc->c1.y);
            mi.y = min(mi.y, q);
            ma.y = max(ma.y, q);
        }
    }

    if (h.z > 0.0f)
    {
        h.z = sqrt(h.z);
        uniform float t = (-b.z - h.z) / a.z;
        if (!isnan(t) && t > 0.0f && t < 1.0f)
        {
            uniform float q = bezier(t, bc->p0.z, bc->c0.z, bc->p1.z, bc->c1.z);
            mi.z = min(mi.z, q);
            ma.z = max(ma.z, q);
        }
        t = (-b.z + h.z) / a.z;
        if (!isnan(t) && t > 0.0f && t < 1.0f)
        {
            uniform float q = bezier(t, bc->p0.z, bc->c0.z, bc->p1.z, bc->c1.z);
            mi.z = min(mi.z, q);
            ma.z = max(ma.z, q);
        }
    }

    return make_box3fa(mi - r, ma + r);
#endif
}
//////////////////////////////////////////////////////////////////////

#define MIN_SEGMENT_COUNT 1
#define MAX_SEGMENT_COUNT 6

int screenAwareSegmentCount(const Bezier_ptr bc, const SDFParams& params)
{
    float costhetaEye =
        dot(normalize(bc->p0 - params.eye), normalize(bc->p1 - params.eye));

    float ratio = (1.0f - costhetaEye) / 0.5f; // cos(vfov / 180 * M_PI);

    return clamp((int)(MIN_SEGMENT_COUNT +
                       (30 * sqrt(ratio)) *
                           (MAX_SEGMENT_COUNT - MIN_SEGMENT_COUNT)),
                 MIN_SEGMENT_COUNT, MAX_SEGMENT_COUNT) *
           params.segmentCountMultiplier;
}

#undef MIN_SEGMENT_COUNT
#undef MAX_SEGMENT_COUNT

//////////////////////////////////////////////////////////////////////

float bezierDistance(const vec3f& p, uDataPtr_t geo_, uDataPtr_t bezier_,
                     const SDFParams& params)
{
    const Geo_ptr geo = (const Geo_ptr)geo_;
    const Bezier_ptr bc = (const Bezier_ptr)bezier_;

    const int segmentCount = screenAwareSegmentCount(bc, params);
    const float deltaT = 1.0f / segmentCount;

    float minDist = 1e10f;

    vec3f prevB = bezier(0.f, bc->p0, bc->c0, bc->p1, bc->c1);

    for (int i = 1; i <= segmentCount; i++)
    {
        const float t = i * deltaT;
        const vec3f b = bezier(t, bc->p0, bc->c0, bc->p1, bc->c1);
        const float d = sdf_tube(p, prevB, b, bc->r0, bc->r1, t, deltaT);
        minDist = min(minDist, d);
        prevB = b;
    }

    return minDist;
}

/////////////////////////////////////////////////////////////////////////////

unmasked void SDFBeziers_bounds(const RTCBoundsFunctionArguments* uniform args)
{
    const Geo_ptr geo = (Geo_ptr)args->geometryUserPtr;
    const Bezier_ptr bezier =
        safeIncrement(geo->useSafeIncrement, geo->data, args->primID);

    *((box3fa * uniform) args->bounds_o) =
        bezierBounds((uDataPtr_t)geo, (uDataPtr_t)bezier);
}

/////////////////////////////////////////////////////////////////////////////

unmasked void SDFBeziers_intersect(
    const RTCIntersectFunctionNArguments* uniform args)
{
    const Geo_ptr geo = (Geo_ptr)args->geometryUserPtr;

    const Bezier_ptr bezier =
        safeIncrement(geo->useSafeIncrement, geo->data, args->primID);

    varying Ray* uniform ray = (varying Ray * uniform) args->rayhit;

    SDFParams sdfParams;
    sdfParams.eye = ray->org;
    sdfParams.segmentCountMultiplier = 1.f;

    const float t_in =
        raymarching(*ray, bezierDistance, bezierBounds, (uDataPtr_t)geo,
                    (uDataPtr_t)bezier, sdfParams);

    if (t_in > 0 && t_in > ray->t0 && t_in < ray->t)
    {
        const vec3f pos = ray->org + t_in * ray->dir;

        ray->primID = args->primID;
        ray->geomID = geo->super.geomID;
        ray->instID = args->context->instID[0];
        ray->t = t_in;

        // increase multiplier for smooth normals
        sdfParams.segmentCountMultiplier = 5.f;

        // TODO epsilon based on pixel radius
        ray->Ng =
            computeNormal(pos, bezierDistance, calcEpsilon(ray->org, t_in),
                          (uDataPtr_t)geo, (uDataPtr_t)bezier, sdfParams);
    }
}

/////////////////////////////////////////////////////////////////////////////

static void SDFBeziers_postIntersect(uniform Geometry* uniform geometry,
                                     uniform Model* uniform model,
                                     varying DifferentialGeometry& dg,
                                     const varying Ray& ray,
                                     uniform int64 flags)
{
    Geo_ptr this = (Geo_ptr)geometry;
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

/////////////////////////////////////////////////////////////////////////////

export void* uniform SDFBeziers_create(void* uniform cppEquivalent)
{
    Geo_ptr geom = uniform new uniform SDFBeziers;
    Geometry_Constructor(&geom->super, cppEquivalent, SDFBeziers_postIntersect,
                         NULL, NULL, 0, NULL);
    return geom;
}

/////////////////////////////////////////////////////////////////////////////

export void SDFBeziersGeometry_set(void* uniform _geo, void* uniform _model,
                                   void* uniform data,
                                   int uniform numPrimitives)
{
    Geo_ptr geo = (Geo_ptr)_geo;
    uniform Model* uniform model = (uniform Model * uniform) _model;

    RTCGeometry geom =
        rtcNewGeometry(ispc_embreeDevice(), RTC_GEOMETRY_TYPE_USER);
    uniform uint32 geomID = rtcAttachGeometry(model->embreeSceneHandle, geom);

    geo->super.model = model;
    geo->super.geomID = geomID;
    geo->super.numPrimitives = numPrimitives;
    geo->data = (uniform SDFBezier * uniform) data;
    geo->useSafeIncrement = needsSafeIncrement(geo->data, numPrimitives);

    rtcSetGeometryUserData(geom, geo);
    rtcSetGeometryUserPrimitiveCount(geom, numPrimitives);
    rtcSetGeometryBoundsFunction(geom,
                                 (uniform RTCBoundsFunction)&SDFBeziers_bounds,
                                 geo);
    rtcSetGeometryIntersectFunction(
        geom, (uniform RTCIntersectFunctionN)&SDFBeziers_intersect);
    rtcSetGeometryOccludedFunction(
        geom, (uniform RTCOccludedFunctionN)&SDFBeziers_intersect);
    rtcCommitGeometry(geom);
    rtcReleaseGeometry(geom);
}
