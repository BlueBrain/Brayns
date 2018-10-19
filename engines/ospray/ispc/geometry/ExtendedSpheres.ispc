/* Copyright (c) 2015-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * Based on OSPRay implementation
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
#include "ospray/SDK/math/sampling.ih"
#include "ospray/SDK/math/vec.ih"
#include "ospray/version.h"

// embree
#include "embree3/rtcore.isph"
#include "embree3/rtcore_geometry.isph"
#include "embree3/rtcore_scene.isph"

#include "utils/SafeIncrement.ih"

#include "brayns/common/geometry/Sphere.h"

DEFINE_SAFE_INCREMENT(Sphere);

struct ExtendedSpheres
{
    uniform Geometry super;

    uniform Sphere* uniform data;

    int32 numExtendedSpheres;
    uniform bool useSafeIncrement;
};

typedef uniform float uniform_float;

static void ExtendedSpheres_postIntersect(uniform Geometry* uniform geometry,
                                          uniform Model* uniform model,
                                          varying DifferentialGeometry& dg,
                                          const varying Ray& ray,
                                          uniform int64 flags)
{
    uniform ExtendedSpheres* uniform self =
        (uniform ExtendedSpheres * uniform)geometry;
    dg.geometry = geometry;
    vec3f Ng = ray.Ng;
    vec3f Ns = Ng;

    // Store timestamp as texture coordinate
    uniform Sphere* sphere =
        safeIncrement(self->useSafeIncrement, self->data, ray.primID);
    const vec2f tex = sphere->texture_coords;

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

unmasked void ExtendedSpheres_bounds(
    const RTCBoundsFunctionArguments* uniform args)
{
    const uniform ExtendedSpheres* uniform self =
        (const uniform ExtendedSpheres * uniform)args->geometryUserPtr;
    const uniform Sphere* uniform sphere =
        safeIncrement(self->useSafeIncrement, self->data, args->primID);

    const uniform float radius = sphere->radius;
    const uniform vec3f center = sphere->center;
    box3fa* uniform bbox = (box3fa* uniform)args->bounds_o;
    *bbox = make_box3fa(center - radius, center + radius);
}

unmasked void ExtendedSpheres_intersect(
    const RTCIntersectFunctionNArguments* uniform args)
{
    if (!args->valid[programIndex]) return;

    const uniform ExtendedSpheres* uniform self =
        (const uniform ExtendedSpheres * uniform)args->geometryUserPtr;
    const uniform unsigned int primID = args->primID;

    const uniform Sphere* uniform sphere =
        safeIncrement(self->useSafeIncrement, self->data, primID);

    const uniform float timestamp = sphere->timestamp;

    varying Ray* uniform ray = (varying Ray* uniform)args->rayhit;

    if (ray->time > 0 && timestamp > ray->time)
        return;

    const uniform float radius = sphere->radius;
    const uniform vec3f center = sphere->center;
    const float approxDist = dot(center - ray->org, ray->dir);
    const vec3f closeOrg = ray->org + approxDist * ray->dir;
    const vec3f A = center - closeOrg;

    const float a = dot(ray->dir, ray->dir);
    const float b = 2.f * dot(ray->dir, A);
    const float c = dot(A, A) - radius * radius;

    const float radical = b * b - 4.f * a * c;
    if (radical < 0.f)
        return;

    const float srad = sqrt(radical);

    const float t_in = (b - srad) * rcpf(2.f * a) + approxDist;
    const float t_out = (b + srad) * rcpf(2.f * a) + approxDist;

    bool hit = false;
    if (t_in > ray->t0 && t_in < ray->t)
    {
        hit = true;
        ray->t = t_in;
    }
    else if (t_out > ray->t0 && t_out < ray->t)
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
        // we need hit in object space, in postIntersect it is in world-space
        ray->Ng = ray->org + ray->t * ray->dir - center;
    }
}

export void* uniform ExtendedSpheres_create(void* uniform cppEquivalent)
{
    uniform ExtendedSpheres* uniform geom = uniform new uniform ExtendedSpheres;
    Geometry_Constructor(&geom->super, cppEquivalent,
                         ExtendedSpheres_postIntersect, NULL, NULL, 0, NULL);
    return geom;
}

export void ExtendedSpheresGeometry_set(void* uniform _self,
                                        void* uniform _model,
                                        void* uniform data,
                                        int uniform numPrimitives)
{
    uniform ExtendedSpheres* uniform self =
        (uniform ExtendedSpheres * uniform)_self;
    uniform Model* uniform model = (uniform Model * uniform)_model;

    RTCGeometry geom =
        rtcNewGeometry(ispc_embreeDevice(), RTC_GEOMETRY_TYPE_USER);
    uniform uint32 geomID = rtcAttachGeometry(model->embreeSceneHandle, geom);

    self->super.model = model;
    self->super.geomID = geomID;
    self->super.numPrimitives = numPrimitives;
    self->data = (uniform Sphere * uniform)data;
    self->useSafeIncrement = needsSafeIncrement(self->data, numPrimitives);

    rtcSetGeometryUserData(geom, self);
    rtcSetGeometryUserPrimitiveCount(geom, numPrimitives);
    rtcSetGeometryBoundsFunction(
        geom, (uniform RTCBoundsFunction)&ExtendedSpheres_bounds, self);
    rtcSetGeometryIntersectFunction(
        geom, (uniform RTCIntersectFunctionN)&ExtendedSpheres_intersect);
    rtcSetGeometryOccludedFunction(
        geom, (uniform RTCOccludedFunctionN)&ExtendedSpheres_intersect);
    rtcCommitGeometry(geom);
    rtcReleaseGeometry(geom);
}
