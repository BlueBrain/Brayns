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
#include "embree3/rtcore.isph"
#include "embree3/rtcore_geometry.isph"
#include "embree3/rtcore_scene.isph"

#include "brayns/common/geometry/Cylinder.h"
#include "utils/SafeIncrement.ih"

DEFINE_SAFE_INCREMENT(Cylinder);

struct ExtendedCylinders
{
    uniform Geometry super; //!< inherited geometry fields

    uniform Cylinder* uniform data;

    int32 numExtendedCylinders;
    uniform bool useSafeIncrement;
};

typedef uniform float uniform_float;

unmasked void ExtendedCylinders_bounds(
    const RTCBoundsFunctionArguments* uniform args)
{
    const uniform ExtendedCylinders* uniform self =
        (uniform ExtendedCylinders * uniform)args->geometryUserPtr;
    const uniform Cylinder* uniform cylinderPtr =
        safeIncrement(self->useSafeIncrement, self->data, args->primID);

    const uniform float radius = cylinderPtr->radius;
    const uniform vec3f v0 = cylinderPtr->center;
    const uniform vec3f v1 = cylinderPtr->up;

    box3fa* uniform bbox = (box3fa * uniform)args->bounds_o;
    *bbox = make_box3fa(min(v0, v1) - make_vec3f(radius),
                        max(v0, v1) + make_vec3f(radius));
}

unmasked void ExtendedCylinders_intersect(
    const RTCIntersectFunctionNArguments *uniform args)
{
    const uniform ExtendedCylinders* uniform self =
        (uniform ExtendedCylinders * uniform)args->geometryUserPtr;
    const uniform int primID = args->primID;

    const uniform Cylinder* uniform cylinderPtr =
        safeIncrement(self->useSafeIncrement, self->data, primID);

    const uniform float timestamp = cylinderPtr->timestamp;

    varying Ray* uniform ray = (varying Ray* uniform)args->rayhit;

    if (ray->time > 0 && timestamp > ray->time)
        return;

    const uniform float radius = cylinderPtr->radius;
    const uniform vec3f v0 = cylinderPtr->center;
    const uniform vec3f v1 = cylinderPtr->up;

    const vec3f center = 0.5f * (v0 + v1);
    const float approxDist = dot(center - ray->org, ray->dir);
    const vec3f closeOrg = ray->org + approxDist * ray->dir;

    const vec3f A = v0 - closeOrg;
    const vec3f B = v1 - closeOrg;

    const vec3f V = ray->dir;
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
    const float tAB0 = max(ray->t0, min(tA, tB));
    const float tAB1 = min(ray->t, max(tA, tB));

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
        ray->t = t_in;
    }
    else if (t_out >= (tAB0) && t_out <= (tAB1))
    {
        hit = true;
        ray->t = t_out;
    }

    if (hit)
    {
        ray->primID = primID;
        ray->geomID = self->super.geomID;
        ray->instID = args->context->instID[0];
        // cannot easily be moved to postIntersect
        // we need hit in object-space, in postIntersect it is in world-space
        const vec3f P = ray->org + ray->t * ray->dir - v0;
        const vec3f V = cross(P, AB);
        ray->Ng = cross(AB, V);
        ray->u = (ray->t - tA) * rcp(tB - tA);
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

    Geometry_Constructor(&geom->super, cppEquivalent,
                         ExtendedCylinders_postIntersect, 0, 0, 0);
    return geom;
}

export void ExtendedCylindersGeometry_set(void* uniform _self,
                                          void* uniform _model,
                                          void* uniform data,
                                          int uniform numPrimitives)
{
    uniform ExtendedCylinders* uniform self =
        (uniform ExtendedCylinders * uniform)_self;
    uniform Model* uniform model = (uniform Model * uniform)_model;

    RTCGeometry geom =
        rtcNewGeometry(ispc_embreeDevice(), RTC_GEOMETRY_TYPE_USER);
    uniform uint32 geomID = rtcAttachGeometry(model->embreeSceneHandle, geom);

    self->super.model = model;
    self->super.geomID = geomID;
    self->super.numPrimitives = numPrimitives;
    self->data = (uniform Cylinder * uniform)data;
    self->useSafeIncrement = needsSafeIncrement(self->data, numPrimitives);

    rtcSetGeometryUserData(geom, self);
    rtcSetGeometryUserPrimitiveCount(geom, numPrimitives);
    rtcSetGeometryBoundsFunction(
        geom, (uniform RTCBoundsFunction)&ExtendedCylinders_bounds, self);
    rtcSetGeometryIntersectFunction(
        geom, (uniform RTCIntersectFunctionN)&ExtendedCylinders_intersect);
    rtcSetGeometryOccludedFunction(
        geom, (uniform RTCOccludedFunctionN)&ExtendedCylinders_intersect);
    rtcCommitGeometry(geom);
    rtcReleaseGeometry(geom);
}
