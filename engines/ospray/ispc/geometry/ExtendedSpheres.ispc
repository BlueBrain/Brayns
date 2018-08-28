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
#include "ospray/SDK/math/vec.ih"

// embree
#include "embree2/rtcore.isph"
#include "embree2/rtcore_geometry_user.isph"
#include "embree2/rtcore_scene.isph"

#include "utils/SafeIncrement.ih"

#include "brayns/common/geometry/Sphere.h"

DEFINE_SAFE_INCREMENT(Sphere);

struct ExtendedSpheres
{
    uniform Geometry geometry;

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
    uniform ExtendedSpheres* uniform this =
        (uniform ExtendedSpheres * uniform)geometry;
    dg.geometry = geometry;
#if ((OSPRAY_VERSION_MAJOR == 1) && (OSPRAY_VERSION_MINOR < 5))
    dg.material = geometry->material;
#endif
    vec3f Ng = ray.Ng;
    vec3f Ns = Ng;

    // Store timestamp as texture coordinate
    uniform Sphere* spherePtr =
        safeIncrement(this->useSafeIncrement, this->data, ray.primID);
    const vec2f tex = spherePtr->texture_coords;

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

void ExtendedSpheres_bounds(uniform ExtendedSpheres* uniform geometry,
                            uniform size_t primID, uniform box3fa& bbox)
{
    uniform Sphere* uniform spherePtr =
        safeIncrement(geometry->useSafeIncrement, geometry->data, primID);

    uniform float radius = spherePtr->radius;

    uniform vec3f center = spherePtr->center;
    bbox =
        make_box3fa(center - make_vec3f(radius), center + make_vec3f(radius));
}

void ExtendedSpheres_intersect(uniform ExtendedSpheres* uniform geometry,
                               varying Ray& ray, uniform size_t primID)
{
    uniform Sphere* uniform spherePtr =
        safeIncrement(geometry->useSafeIncrement, geometry->data, primID);

    uniform float timestamp = spherePtr->timestamp;

    if (ray.time > 0 && timestamp > ray.time)
        return;

    uniform float radius = spherePtr->radius;
    uniform vec3f center = spherePtr->center;
    const float approxDist = dot(center - ray.org, ray.dir);
    const vec3f closeOrg = ray.org + approxDist * ray.dir;
    const vec3f A = center - closeOrg;

    const float a = dot(ray.dir, ray.dir);
    const float b = 2.f * dot(ray.dir, A);
    const float c = dot(A, A) - radius * radius;

    const float radical = b * b - 4.f * a * c;
    if (radical < 0.f)
        return;

    const float srad = sqrt(radical);

    const float t_in = (b - srad) * rcpf(2.f * a) + approxDist;
    const float t_out = (b + srad) * rcpf(2.f * a) + approxDist;

    bool hit = false;
    if (t_in > ray.t0 && t_in < ray.t)
    {
        hit = true;
        ray.t = t_in;
    }
    else if (t_out > ray.t0 && t_out < ray.t)
    {
        hit = true;
        ray.t = t_out;
    }
    if (hit)
    {
        ray.primID = primID;
        ray.geomID = geometry->geometry.geomID;
        // cannot easily be moved to postIntersect
        // we need hit in object space, in postIntersect it is in world-space
        ray.Ng = ray.org + ray.t * ray.dir - center;
    }
}

export void* uniform ExtendedSpheres_create(void* uniform cppEquivalent)
{
    uniform ExtendedSpheres* uniform geom = uniform new uniform ExtendedSpheres;
    Geometry_Constructor(&geom->geometry, cppEquivalent,
                         ExtendedSpheres_postIntersect, 0, 0, 0);
    return geom;
}

export void ExtendedSpheresGeometry_set(void* uniform _geom,
                                        void* uniform _model,
                                        void* uniform data,
                                        int uniform numExtendedSpheres)
{
    uniform ExtendedSpheres* uniform geom =
        (uniform ExtendedSpheres * uniform)_geom;
    uniform Model* uniform model = (uniform Model * uniform)_model;

    uniform uint32 geomID =
        rtcNewUserGeometry(model->embreeSceneHandle, numExtendedSpheres);

    geom->geometry.model = model;
    geom->geometry.geomID = geomID;
    geom->numExtendedSpheres = numExtendedSpheres;
    geom->data = (uniform Sphere * uniform)data;
    geom->useSafeIncrement = needsSafeIncrement(geom->data, numExtendedSpheres);

    rtcSetUserData(model->embreeSceneHandle, geomID, geom);
    rtcSetBoundsFunction(model->embreeSceneHandle, geomID,
                         (uniform RTCBoundsFunc)&ExtendedSpheres_bounds);
    rtcSetIntersectFunction(
        model->embreeSceneHandle, geomID,
        (uniform RTCIntersectFuncVarying)&ExtendedSpheres_intersect);
    rtcSetOccludedFunction(
        model->embreeSceneHandle, geomID,
        (uniform RTCOccludedFuncVarying)&ExtendedSpheres_intersect);
    rtcEnable(model->embreeSceneHandle, geomID);
}
