/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
 */

#include "StereoCamera.h"
#include "StereoCamera_ispc.h"
#include <limits>

#define OSP_REGISTER_EXCAMERA(InternalClassName,external_name) \
extern "C" ospray::Camera *ospray_create_camera__##external_name()   \
{                                                            \
    return new InternalClassName;                            \
}

namespace brayns
{

StereoCamera::StereoCamera()
{
    ispcEquivalent = ispc::StereoCamera_create(this);
}

void StereoCamera::commit()
{
    pos = getParam3f("pos", ospray::vec3fa(0.f, 0.f, 0.f));
    dir = getParam3f("dir", ospray::vec3fa(0.f, 0.f, 1.f));
    up = getParam3f("up", ospray::vec3fa(0.f, 1.f, 0.f));
    near = getParamf("near",0.f);
    far = getParamf("far", std::numeric_limits<float>::infinity());
    fovy = getParamf("fovy",60.f);
    aspect = getParamf("aspect",1.f);
    nearClip = getParam1f("near_clip",getParam1f("nearClip",1e-6f));
    stereo = getParamf("stereo",0.f);
    separation = getParamf("separation",0.f);

    ospray::vec3f dz = normalize(dir);
    ospray::vec3f dx = normalize(cross(dz,up));
    ospray::vec3f dy = normalize(cross(dx,dz));

    float imgPlane_size_y = 2.f*sinf(fovy/2.f*M_PI/180.);
    float imgPlane_size_x = imgPlane_size_y * aspect;
    dir_00 = dz
            - (.5f * imgPlane_size_x) * dx
            - (.5f * imgPlane_size_y) * dy;
    dir_du = dx * imgPlane_size_x;
    dir_dv = dy * imgPlane_size_y;

    ispc::StereoCamera_set(getIE(),
                           (const ispc::vec3f&)pos,
                           (const ispc::vec3f&)dir_00,
                           (const ispc::vec3f&)dir_du,
                           (const ispc::vec3f&)dir_dv,
                           nearClip, stereo, separation);
}

void StereoCamera::initRay(ospray::Ray &ray, const osp::vec2f &sample)
{
    ray.org = pos;
    ray.dir = dir_00
            + sample.x * dir_du / 2
            + sample.y * dir_dv;

    ray.dir = normalize(ray.dir);

    if (ray.dir.x == 0.f) ray.dir.x = 1e-6f;
    if (ray.dir.y == 0.f) ray.dir.y = 1e-6f;
    if (ray.dir.z == 0.f) ray.dir.z = 1e-6f;


    ray.t0  = 1e-6f;
    ray.t   = std::numeric_limits<float>::infinity();
    ray.time = 0.f;
    ray.mask = -1;
    ray.geomID = -1;
    ray.primID = -1;
    ray.instID = -1;
}

OSP_REGISTER_EXCAMERA(StereoCamera,stereo);
} // ::brayns
