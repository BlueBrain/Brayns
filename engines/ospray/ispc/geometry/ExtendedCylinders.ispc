/* Copyright (c) 2015-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
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
// embree
#include "embree2/rtcore.isph"
#include "embree2/rtcore_geometry_user.isph"
#include "embree2/rtcore_scene.isph"

#include "brayns/common/geometry/Cylinder.h"
#include "utils/SafeIncrement.ih"

DEFINE_SAFE_INCREMENT(Cylinder);

struct ExtendedCylinders
{
    uniform Geometry geometry; //!< inherited geometry fields

    uniform Cylinder* uniform data;

    int32 numExtendedCylinders;
    uniform bool useSafeIncrement;
};

typedef uniform float uniform_float;

void ExtendedCylinders_bounds(uniform ExtendedCylinders* uniform geometry,
                              uniform size_t primID, uniform box3fa& bbox)
{
    uniform Cylinder* uniform cylinderPtr =
        safeIncrement(geometry->useSafeIncrement, geometry->data, primID);
    uniform float radius = cylinderPtr->radius;

    uniform vec3f v0 = cylinderPtr->center;
    uniform vec3f v1 = cylinderPtr->up;
    bbox = make_box3fa(min(v0, v1) - make_vec3f(radius),
                       max(v0, v1) + make_vec3f(radius));
}

void ExtendedCylinders_intersect(uniform ExtendedCylinders* uniform geometry,
                                 varying Ray& ray, uniform size_t primID)
{
    uniform Cylinder* uniform cylinderPtr =
        safeIncrement(geometry->useSafeIncrement, geometry->data, primID);

    uniform float timestamp = cylinderPtr->timestamp;

    if (ray.time > 0 && timestamp > ray.time)
        return;

    uniform float radius = cylinderPtr->radius;
    uniform vec3f v0 = cylinderPtr->center;
    uniform vec3f v1 = cylinderPtr->up;

    const vec3f center = 0.5f * (v0 + v1);
    const float approxDist = dot(center - ray.org, ray.dir);
    const vec3f closeOrg = ray.org + approxDist * ray.dir;

    const vec3f A = v0 - closeOrg;
    const vec3f B = v1 - closeOrg;

    const vec3f V = ray.dir;
    const vec3f AB = B - A;

    const vec3f AOxAB = cross(AB, A);
    const vec3f VxAB = cross(V, AB);
    const float ab2 = dot(AB, AB);
    const float a = dot(VxAB, VxAB);
    const float b = 2 * dot(VxAB, AOxAB);
    const float c = dot(AOxAB, AOxAB) - (sqr(radius) * ab2);

    // clip to near and far cap of cylinder
    const float rVAB = rcp(dot(V, AB));
    const float tA = dot(AB, A) * rVAB + approxDist;
    const float tB = dot(AB, B) * rVAB + approxDist;
    const float tAB0 = max(ray.t0, min(tA, tB));
    const float tAB1 = min(ray.t, max(tA, tB));

    // ------------------------------------------------------------------
    // abc formula: t0,1 = (-b +- sqrt(b^2-4*a*c)) / 2a
    //
    const float radical = b * b - 4.f * a * c;
    if (radical < 0.f)
        return;

    const float srad = sqrt(radical);

    const float t_in = (-b - srad) * rcpf(2.f * a) + approxDist;
    const float t_out = (-b + srad) * rcpf(2.f * a) + approxDist;

    bool hit = false;

    if (t_in >= (tAB0) && t_in <= (tAB1))
    {
        hit = true;
        ray.t = t_in;
    }
    else if (t_out >= (tAB0) && t_out <= (tAB1))
    {
        hit = true;
        ray.t = t_out;
    }

    if (hit)
    {
        ray.primID = primID;
        ray.geomID = geometry->geometry.geomID;
        // cannot easily be moved to postIntersect
        // we need hit in object-space, in postIntersect it is in world-space
        const vec3f P = ray.org + ray.t * ray.dir - v0;
        const vec3f V = cross(P, AB);
        ray.Ng = cross(AB, V);
        ray.u = (ray.t - tA) * rcp(tB - tA);
    }
}

static void ExtendedCylinders_postIntersect(uniform Geometry* uniform geometry,
                                            uniform Model* uniform model,
                                            varying DifferentialGeometry& dg,
                                            const varying Ray& ray,
                                            uniform int64 flags)
{
    uniform ExtendedCylinders* uniform this =
        (uniform ExtendedCylinders * uniform)geometry;
    dg.geometry = geometry;
#if ((OSPRAY_VERSION_MAJOR == 1) && (OSPRAY_VERSION_MINOR < 5))
    dg.material = geometry->material;
#endif
    vec3f Ng = ray.Ng;
    vec3f Ns = Ng;

    uniform Cylinder* cylinderPtr =
        safeIncrement(this->useSafeIncrement, this->data, ray.primID);
    // Store value as texture coordinate
    const vec2f tex = cylinderPtr->texture_coords;

    dg.st.x = tex.x;
    dg.st.y = tex.y;

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

export void* uniform ExtendedCylinders_create(void* uniform cppEquivalent)
{
    uniform ExtendedCylinders* uniform geom =
        uniform new uniform ExtendedCylinders;

    Geometry_Constructor(&geom->geometry, cppEquivalent,
                         ExtendedCylinders_postIntersect, 0, 0, 0);
    return geom;
}

export void ExtendedCylindersGeometry_set(void* uniform _geom,
                                          void* uniform _model,
                                          void* uniform data,
                                          int uniform numExtendedCylinders)
{
    uniform ExtendedCylinders* uniform geom =
        (uniform ExtendedCylinders * uniform)_geom;
    uniform Model* uniform model = (uniform Model * uniform)_model;

    uniform uint32 geomID =
        rtcNewUserGeometry(model->embreeSceneHandle, numExtendedCylinders);

    geom->geometry.model = model;
    geom->geometry.geomID = geomID;
    geom->numExtendedCylinders = numExtendedCylinders;
    geom->data = (uniform Cylinder * uniform)data;
    geom->useSafeIncrement =
        needsSafeIncrement(geom->data, numExtendedCylinders);

    rtcSetUserData(model->embreeSceneHandle, geomID, geom);
    rtcSetBoundsFunction(model->embreeSceneHandle, geomID,
                         (uniform RTCBoundsFunc)&ExtendedCylinders_bounds);
    rtcSetIntersectFunction(
        model->embreeSceneHandle, geomID,
        (uniform RTCIntersectFuncVarying)&ExtendedCylinders_intersect);
    rtcSetOccludedFunction(
        model->embreeSceneHandle, geomID,
        (uniform RTCOccludedFuncVarying)&ExtendedCylinders_intersect);
    rtcEnable(model->embreeSceneHandle, geomID);
}
