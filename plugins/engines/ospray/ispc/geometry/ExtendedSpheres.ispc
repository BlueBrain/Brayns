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

struct ExtendedSpheres
{
    uniform Geometry geometry;

    uniform uint8 *uniform data;
    uniform Material *uniform *materialList;

    float radius;
    int materialID;
    int offset_center;
    int offset_radius;
    int offset_timestamp;
    int offset_texture_coords;
    int offset_materialID;
    int32 numExtendedSpheres;
    int32 bytesPerExtendedSphere;
};

typedef uniform float uniform_float;

static void ExtendedSpheres_postIntersect(uniform Geometry *uniform geometry,
                                          uniform Model *uniform model,
                                          varying DifferentialGeometry &dg,
                                          const varying Ray &ray,
                                          uniform int64 flags)
{
    uniform ExtendedSpheres *uniform this =
        (uniform ExtendedSpheres * uniform)geometry;
    dg.geometry = geometry;
#if ((OSPRAY_VERSION_MAJOR == 1) && (OSPRAY_VERSION_MINOR < 5))
    dg.material = geometry->material;
#endif
    vec3f Ng = ray.Ng;
    vec3f Ns = Ng;

    // Store timestamp as texture coordinate
    uniform uint8 *spherePtr =
        this->data + this->bytesPerExtendedSphere * ray.primID;
    const vec2f tex =
        *((uniform vec2f *)(spherePtr + this->offset_texture_coords));

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
    if ((flags & DG_MATERIALID) && (this->offset_materialID >= 0))
    {
        const uniform int32 primsPerPage = (1024 * 1024 * 128);
        if (any(ray.primID >= primsPerPage))
        {
            const int primPageID = ray.primID / primsPerPage;
            const int localPrimID = ray.primID % primsPerPage;
            foreach_unique(primPage in primPageID)
            {
                uniform uint8 *uniform pagePtr =
                    this->data + (((int64)primPage) * primsPerPage *
                                  this->bytesPerExtendedSphere);

                uniform uint8 *spherePtr =
                    pagePtr + this->bytesPerExtendedSphere * localPrimID;

                dg.materialID = *((uniform uint32 * varying)(
                    spherePtr + this->offset_materialID));

                if (this->materialList)
                    dg.material = this->materialList[dg.materialID];
            }
        }
        else
        {
            uniform uint8 *spherePtr =
                this->data + this->bytesPerExtendedSphere * ray.primID;
            dg.materialID = *((uniform uint32 *
                               varying)(spherePtr + this->offset_materialID));

            if (this->materialList)
                dg.material = this->materialList[dg.materialID];
        }
    }
    dg.Ng = Ng;
    dg.Ns = Ns;
}

void ExtendedSpheres_bounds(uniform ExtendedSpheres *uniform geometry,
                            uniform size_t primID, uniform box3fa &bbox)
{
    uniform uint8 *uniform spherePtr =
        geometry->data +
        geometry->bytesPerExtendedSphere * ((uniform int64)primID);

    uniform float radius = geometry->radius;
    if (geometry->offset_radius >= 0)
        radius = *((uniform float *)(spherePtr + geometry->offset_radius));

    uniform vec3f center =
        *((uniform vec3f *)(spherePtr + geometry->offset_center));
    bbox =
        make_box3fa(center - make_vec3f(radius), center + make_vec3f(radius));
}

void ExtendedSpheres_intersect(uniform ExtendedSpheres *uniform geometry,
                               varying Ray &ray, uniform size_t primID)
{
    uniform uint8 *uniform spherePtr =
        geometry->data +
        geometry->bytesPerExtendedSphere * ((uniform int64)primID);

    uniform float timestamp =
        *((uniform float *)(spherePtr + geometry->offset_timestamp));

    if (ray.time > 0 && timestamp > ray.time)
        return;

    uniform float radius = geometry->radius;
    if (geometry->offset_radius >= 0)
        radius = *((uniform float *)(spherePtr + geometry->offset_radius));

    uniform vec3f center =
        *((uniform vec3f *)(spherePtr + geometry->offset_center));
    const vec3f A = center - ray.org;

    const float a = dot(ray.dir, ray.dir);
    const float b = -2.f * dot(ray.dir, A);
    const float c = dot(A, A) - radius * radius;

    const float radical = b * b - 4.f * a * c;
    if (radical < 0.f)
        return;

    const float srad = sqrt(radical);

    const float t_in = (-b - srad) * rcpf(2.f * a);
    const float t_out = (-b + srad) * rcpf(2.f * a);

    if (t_in > ray.t0 && t_in < ray.t)
    {
        ray.primID = primID;
        ray.geomID = geometry->geometry.geomID;
        ray.t = t_in;
        ray.Ng = ray.org + ray.t * ray.dir - center;
        return;
    }
    else if (t_out > ray.t0 && t_out < ray.t)
    {
        ray.primID = primID;
        ray.geomID = geometry->geometry.geomID;
        ray.t = t_out;
        ray.Ng = ray.org + ray.t * ray.dir - center;
        return;
    }
    return;
}

export void *uniform ExtendedSpheres_create(void *uniform cppEquivalent)
{
    uniform ExtendedSpheres *uniform geom = uniform new uniform ExtendedSpheres;
    Geometry_Constructor(&geom->geometry, cppEquivalent,
                         ExtendedSpheres_postIntersect, 0, 0, 0);
    return geom;
}

export void ExtendedSpheresGeometry_set(
    void *uniform _geom, void *uniform _model, void *uniform data,
    void *uniform materialList, int uniform numExtendedSpheres,
    int uniform bytesPerExtendedSphere, float uniform radius,
    int uniform materialID, int uniform offset_center,
    int uniform offset_radius, int uniform offset_timestamp,
    int uniform offset_texture_coords, int uniform offset_materialID)
{
    uniform ExtendedSpheres *uniform geom =
        (uniform ExtendedSpheres * uniform)_geom;
    uniform Model *uniform model = (uniform Model * uniform)_model;

    uniform uint32 geomID =
        rtcNewUserGeometry(model->embreeSceneHandle, numExtendedSpheres);

    geom->geometry.model = model;
    geom->geometry.geomID = geomID;
    geom->materialList = (Material **)materialList;
    geom->numExtendedSpheres = numExtendedSpheres;
    geom->radius = radius;
    geom->data = (uniform uint8 * uniform)data;
    geom->materialID = materialID;
    geom->bytesPerExtendedSphere = bytesPerExtendedSphere;

    geom->offset_center = offset_center;
    geom->offset_radius = offset_radius;
    geom->offset_timestamp = offset_timestamp;
    geom->offset_texture_coords = offset_texture_coords;
    geom->offset_materialID = offset_materialID;

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
