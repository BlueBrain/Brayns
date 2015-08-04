/* Copyright (c) 2011-2015, EPFL/Blue Brain Project
 *                     Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
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
#include "ospray/math/vec.ih"
#include "ospray/math/box.ih"
#include "ospray/common/Ray.ih"
#include "ospray/common/Model.ih"
#include "ospray/geometry/Geometry.ih"
// embree
#include "embree2/rtcore.isph"
#include "embree2/rtcore_scene.isph"
#include "embree2/rtcore_geometry_user.isph"

struct ExtendedCones
{
    uniform Geometry geometry;

    uniform uint8 *uniform data;

    float radius;
    float length;
    int   materialID;
    int   offset_center;
    int   offset_up;
    int   offset_radius;
    int   offset_length;
    int   offset_frame;
    int   offset_materialID;
    int32 numExtendedCones;
    int32 bytesPerCone;
};

typedef uniform float uniform_float;

void ExtendedCones_bounds(uniform ExtendedCones *uniform geometry,
                          uniform size_t primID,
                          uniform box3fa &bbox)
{
    uniform uint8 *uniform conePtr =
            geometry->data + geometry->bytesPerCone*primID;
    uniform float radius = geometry->radius;
    if (geometry->offset_radius >= 0)
        radius = *((uniform float *)(conePtr+geometry->offset_radius));

    uniform vec3f v0 = *((uniform vec3f*)(conePtr+geometry->offset_center));
    uniform vec3f v1 = *((uniform vec3f*)(conePtr+geometry->offset_up));
    bbox = make_box3fa(min(v0,v1)-make_vec3f(radius),
                       max(v0,v1)+make_vec3f(radius));
}

void ExtendedCones_intersect(uniform ExtendedCones *uniform geometry,
                             varying Ray &ray,
                             uniform size_t primID)
{
    uniform uint8 *uniform conePtr =
            geometry->data + geometry->bytesPerCone*primID;

    uniform float radius = geometry->radius;
    if (geometry->offset_radius >= 0)
        radius = *((uniform float *)(conePtr+geometry->offset_radius));

    uniform float length = geometry->length;
    if (geometry->offset_length >= 0)
        length = *((uniform float *)(conePtr+geometry->offset_length));

    uniform float frame = *((uniform float *)(conePtr+geometry->offset_frame));
    if( frame>ray.time ) return;

    uniform vec3f v0 = *((uniform vec3f*)(conePtr+geometry->offset_center));
    uniform vec3f v1 = v0 + *((uniform vec3f*)(conePtr+geometry->offset_up));
    const vec3f A = v0 - ray.org;
    const vec3f B = v1 - ray.org;
    const float r = radius;

    const vec3f O = make_vec3f(0.f);
    const vec3f V = ray.dir;

    const vec3f AB = B - A;
    const vec3f AO = O - A;

    const vec3f AOxAB  = cross(AO,AB);
    const vec3f VxAB   = cross(V,AB);
    const float ab2    = dot(AB,AB);
    const float a      = dot(VxAB,VxAB);
    const float b      = 2 * dot(VxAB,AOxAB);
    const float c      = dot(AOxAB,AOxAB) - (r*r * ab2);

    const float tA = dot(AB,A) * rcp(dot(V,AB));
    const float tB = dot(AB,B) * rcp(dot(V,AB));
    const float tAB0 = max(ray.t0,min(tA,tB));
    const float tAB1 = min(ray.t, max(tA,tB));

    const float radical = b*b-4.f*a*c;
    if (radical < 0.f) return;

    const float srad = sqrt(radical);

    const float t_in = (- b - srad) *rcpf(2.f*a);
    const float t_out= (- b + srad) *rcpf(2.f*a);

    if (t_in >= tAB0 && t_in <= tAB1) {
        ray.primID = primID;
        ray.geomID = geometry->geometry.geomID;
        ray.t = t_in;
        const vec3f P = ray.org+ray.t*ray.dir - v0;
        // const vec3f P = ray.t*ray.dir - A;
        const vec3f V = cross(P,AB);
        ray.Ng = cross(AB,V);
        return;
    } else if (t_out >= tAB0 && t_out <= tAB1) {
        ray.primID = primID;
        ray.geomID = geometry->geometry.geomID;
        ray.t = t_out;
        const vec3f P = ray.t*ray.dir - A;
        const vec3f V = cross(P,AB);
        ray.Ng = cross(AB,V);
        return;
    }
    return;
}


static void ExtendedCones_postIntersect(uniform Geometry *uniform geometry,
                                        uniform Model *uniform model,
                                        varying DifferentialGeometry &dg,
                                        const varying Ray &ray,
                                        uniform int64 flags)
{
    uniform ExtendedCones *uniform THIS =
            (uniform ExtendedCones *uniform)geometry;
    dg.geometry = geometry;
    dg.material = geometry->material;
    vec3f Ng = ray.Ng;
    vec3f Ns = Ng;

    if (flags & DG_NORMALIZE)\
    {
        Ng = normalize(Ng);
        Ns = normalize(Ns);
    }
    if (flags & DG_FACEFORWARD)
    {
        if (dot(ray.dir,Ng) >= 0.f) Ng = neg(Ng);
        if (dot(ray.dir,Ns) >= 0.f) Ns = neg(Ns);
    }
    if ((flags & DG_MATERIALID) && (THIS->offset_materialID >= 0))
    {
        uniform uint8 *conePtr = THIS->data + THIS->bytesPerCone*ray.primID;
        dg.materialID =
                *((uniform uint32 *varying)(conePtr+THIS->offset_materialID));
    }
    dg.Ng = Ng;
    dg.Ns = Ns;
}

export void *uniform ExtendedCones_create(void *uniform cppEquivalent)
{
    uniform ExtendedCones *uniform geom = uniform new uniform ExtendedCones;
    Geometry_Constructor(&geom->geometry,cppEquivalent,
                         ExtendedCones_postIntersect,
                         NULL,0,NULL);
    return geom;
}

export void ExtendedConesGeometry_set(void *uniform _geom,
                                      void *uniform _model,
                                      void *uniform data,
                                      int   uniform numExtendedCones,
                                      int   uniform bytesPerCone,
                                      float uniform radius,
                                      float uniform length,
                                      int   uniform materialID,
                                      int   uniform offset_center,
                                      int   uniform offset_up,
                                      int   uniform offset_radius,
                                      int   uniform offset_length,
                                      int   uniform offset_frame,
                                      int   uniform offset_materialID)
{
    uniform ExtendedCones *uniform geom = (uniform ExtendedCones *uniform)_geom;
    uniform Model *uniform model = (uniform Model *uniform)_model;

    uniform uint32 geomID =
            rtcNewUserGeometry(model->embreeSceneHandle,numExtendedCones);

    geom->geometry.model = model;
    geom->geometry.geomID = geomID;
    geom->numExtendedCones = numExtendedCones;
    geom->radius = radius;
    geom->length = length;
    geom->data = (uniform uint8 *uniform)data;
    geom->materialID = materialID;
    geom->bytesPerCone = bytesPerCone;

    geom->offset_center     = offset_center;
    geom->offset_up         = offset_up;
    geom->offset_radius     = offset_radius;
    geom->offset_length     = offset_length;
    geom->offset_frame      = offset_frame;
    geom->offset_materialID = offset_materialID;

    rtcSetUserData(model->embreeSceneHandle,geomID,geom);
    rtcSetBoundsFunction(
                model->embreeSceneHandle,geomID,
                (uniform RTCBoundsFunc)&ExtendedCones_bounds);
    rtcSetIntersectFunction(
                model->embreeSceneHandle,geomID,
                (uniform RTCIntersectFuncVarying)&ExtendedCones_intersect);
    rtcSetOccludedFunction(
                model->embreeSceneHandle,geomID,
                (uniform RTCOccludedFuncVarying)&ExtendedCones_intersect);
    rtcEnable(model->embreeSceneHandle,geomID);
}
