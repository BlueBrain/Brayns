/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Author: Jafet Villafranca Diaz <jafet.villafrancadiaz@epfl.ch>
 *
 * Ray-cone intersection:
 * based on Ching-Kuang Shene (Graphics Gems 5, p. 227-230)
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

#include "ospray/SDK/math/vec.ih"

#include "utils/SafeIncrement.ih"

#include "brayns/common/geometry/Cone.h"

DEFINE_SAFE_INCREMENT(Cone);

struct Cones
{
    uniform Geometry super;

    uniform Cone* uniform data;

    uniform bool useSafeIncrement;
};

unmasked void Cones_bounds(const RTCBoundsFunctionArguments* uniform args)
{
    const uniform Cones* uniform self =
        (uniform Cones * uniform)args->geometryUserPtr;
    const uniform Cone* uniform conePtr =
        safeIncrement(self->useSafeIncrement, self->data, args->primID);

    uniform float extent = conePtr->centerRadius;
    const uniform float upRadius = conePtr->upRadius;

    if (upRadius > extent)
        extent = upRadius;

    const uniform vec3f v0 = conePtr->center;
    const uniform vec3f v1 = conePtr->up;

    box3fa* uniform bbox = (box3fa * uniform)args->bounds_o;
    *bbox = make_box3fa(min(v0, v1) - make_vec3f(extent),
                        max(v0, v1) + make_vec3f(extent));
}

unmasked void Cones_intersect(
    const RTCIntersectFunctionNArguments* uniform args)
{
    const uniform Cones* uniform self =
        (uniform Cones * uniform)args->geometryUserPtr;
    const uniform int primID = args->primID;

    const uniform Cone* uniform conePtr =
        safeIncrement(self->useSafeIncrement, self->data, primID);

    uniform float radius0 = conePtr->centerRadius;
    uniform float radius1 = conePtr->upRadius;

    uniform vec3f v0 = conePtr->center;
    uniform vec3f v1 = conePtr->up;

    if (radius0 < radius1)
    {
        // swap radii and positions, so radius0 and v0 are always at the bottom
        uniform float tmpRadius = radius1;
        radius1 = radius0;
        radius0 = tmpRadius;

        uniform vec3f tmpPos = v1;
        v1 = v0;
        v0 = tmpPos;
    }

    const vec3f upVector = v1 - v0;
    const float upLength = length(upVector);

    // Compute the height of the full cone, in order to obtain its vertex
    const float deltaRadius = radius0 - radius1;
    const float tanA = deltaRadius / upLength;
    const float coneHeight = radius0 / tanA;
    const float squareTanA = tanA * tanA;
    const float div = sqrtf(1.f + squareTanA);
    if (div == 0.f)
        return;
    const float cosA = 1.f / div;

    const vec3f V = v0 + normalize(upVector) * coneHeight;
    const vec3f v = normalize(v0 - V);

    // Normal of the plane P determined by V and ray
    varying Ray* uniform ray = (varying Ray * uniform)args->rayhit;
    vec3f n = normalize(cross(ray->dir, V - ray->org));
    const float dotNV = dot(n, v);
    if (dotNV > 0.f)
        n = neg(n);

    const float squareCosTheta = 1.f - dotNV * dotNV;
    const float cosTheta = sqrtf(squareCosTheta);
    if (cosTheta < cosA)
        return; // no intersection

    if (squareCosTheta == 0.f)
        return;

    const float squareTanTheta = (1.f - squareCosTheta) / squareCosTheta;
    const float tanTheta = sqrtf(squareTanTheta);

    // Compute u-v-w coordinate system
    const vec3f u = normalize(cross(v, n));
    const vec3f w = normalize(cross(u, v));

    // Circle intersection of cone with plane P
    const vec3f uComponent = sqrtf(squareTanA - squareTanTheta) * u;
    const vec3f vwComponent = v + tanTheta * w;
    const vec3f delta1 = vwComponent + uComponent;
    const vec3f delta2 = vwComponent - uComponent;
    const vec3f rayApex = V - ray->org;

    const vec3f normal1 = cross(ray->dir, delta1);
    const float length1 = length(normal1);

    if (length1 == 0.f)
        return;

    const float r1 = dot(cross(rayApex, delta1), normal1) / (length1 * length1);

    const vec3f normal2 = cross(ray->dir, delta2);
    const float length2 = length(normal2);

    if (length2 == 0.f)
        return;

    const float r2 = dot(cross(rayApex, delta2), normal2) / (length2 * length2);

    float t_in = r1;
    float t_out = r2;
    if (r2 > 0.f)
    {
        if (r1 > 0.f)
        {
            if (r1 > r2)
            {
                t_in = r2;
                t_out = r1;
            }
        }
        else
            t_in = r2;
    }

    if (t_in > ray->t0 && t_in < ray->t)
    {
        const vec3f p1 = ray->org + t_in * ray->dir;
        // consider only the parts within the extents of the truncated cone
        if (dot(p1 - v1, v) > 0.f && dot(p1 - v0, v) < 0.f)
        {
            ray->primID = primID;
            ray->geomID = self->super.geomID;
            ray->instID = args->context->instID[0];
            ray->t = t_in;
            const vec3f surfaceVec = normalize(p1 - V);
            ray->Ng = cross(cross(v, surfaceVec), surfaceVec);
            return;
        }
    }
    if (t_out > ray->t0 && t_out < ray->t)
    {
        const vec3f p2 = ray->org + t_out * ray->dir;
        // consider only the parts within the extents of the truncated cone
        if (dot(p2 - v1, v) > 0.f && dot(p2 - v0, v) < 0.f)
        {
            ray->primID = primID;
            ray->geomID = self->super.geomID;
            ray->instID = args->context->instID[0];
            ray->t = t_out;
            const vec3f surfaceVec = normalize(p2 - V);
            ray->Ng = cross(cross(v, surfaceVec), surfaceVec);
        }
    }
    return;
}

static void Cones_postIntersect(uniform Geometry* uniform geometry,
                                uniform Model* uniform model,
                                varying DifferentialGeometry& dg,
                                const varying Ray& ray, uniform int64 flags)
{
    uniform Cones* uniform this = (uniform Cones * uniform)geometry;
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

export void* uniform Cones_create(void* uniform cppEquivalent)
{
    uniform Cones* uniform geom = uniform new uniform Cones;
    Geometry_Constructor(&geom->super, cppEquivalent, Cones_postIntersect, NULL,
                         NULL, 0, NULL);
    return geom;
}

export void ConesGeometry_set(void* uniform _self, void* uniform _model,
                              void* uniform data, int uniform numPrimitives)
{
    uniform Cones* uniform self = (uniform Cones * uniform)_self;
    uniform Model* uniform model = (uniform Model * uniform)_model;

    RTCGeometry geom =
        rtcNewGeometry(ispc_embreeDevice(), RTC_GEOMETRY_TYPE_USER);
    uniform uint32 geomID = rtcAttachGeometry(model->embreeSceneHandle, geom);

    self->super.model = model;
    self->super.geomID = geomID;
    self->super.numPrimitives = numPrimitives;
    self->data = (uniform Cone * uniform)data;
    self->useSafeIncrement = needsSafeIncrement(self->data, numPrimitives);

    rtcSetGeometryUserData(geom, self);
    rtcSetGeometryUserPrimitiveCount(geom, numPrimitives);
    rtcSetGeometryBoundsFunction(geom, (uniform RTCBoundsFunction)&Cones_bounds,
                                 self);
    rtcSetGeometryIntersectFunction(
        geom, (uniform RTCIntersectFunctionN)&Cones_intersect);
    rtcSetGeometryOccludedFunction(
        geom, (uniform RTCOccludedFunctionN)&Cones_intersect);
    rtcCommitGeometry(geom);
    rtcReleaseGeometry(geom);
}
