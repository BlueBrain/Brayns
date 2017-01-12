/* Copyright (c) 2015-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
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

OSP_REGISTER_EXCAMERA(StereoCamera,stereo);
} // ::brayns
