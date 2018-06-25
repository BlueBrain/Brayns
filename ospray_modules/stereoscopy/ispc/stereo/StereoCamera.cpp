/* Copyright (c) 2018, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *                     Grigori Chevtchenko <grigori.chevtchenko@epfl.ch>
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

const float distanceToPlane = 10000.0f;

namespace ospray
{
StereoCamera::StereoCamera()
{
    ispcEquivalent = ispc::StereoCamera_create(this);
}

std::string StereoCamera::toString() const
{
    return "ospray::CylindricCamera";
}

void StereoCamera::commit()
{
    Camera::commit();

    const float fovy = getParamf("fovy", 60.f);
    const float fovx = getParamf("fovx", 90.f);
    const StereoMode stereoMode = (StereoMode)getParam1i("stereoMode", OSP_STEREO_NONE);
    const float interpupillaryDistance = getParamf("interpupillaryDistance", 0.0635f);

    float ipd = 0.0f;
    bool sideBySide = false;
    switch (stereoMode)
    {
    case OSP_STEREO_LEFT:
        ipd = -interpupillaryDistance;
        break;
    case OSP_STEREO_RIGHT:
        ipd = interpupillaryDistance;
        break;
    case OSP_STEREO_SIDE_BY_SIDE:
        ipd = interpupillaryDistance;
        sideBySide = true;
        break;
    case OSP_STEREO_NONE:
        break;
    }


    dir = normalize(dir);
    const vec3f dir_du = normalize(cross(dir, up));
    const vec3f dir_dv = normalize(up);
    dir = -dir;

    const vec3f org = pos;
    const float imgPlane_size_y = 2.0f * distanceToPlane * tanf(deg2rad(0.5f * fovy));
    const float imgPlane_size_x = 2.0f * distanceToPlane * tanf(deg2rad(0.5f * fovx));

    ispc::StereoCamera_set(getIE(), (const ispc::vec3f&)org,
                                    (const ispc::vec3f&)dir,
                                    (const ispc::vec3f&)dir_du,
                                    (const ispc::vec3f&)dir_dv,
                                    distanceToPlane, imgPlane_size_y,
                                    imgPlane_size_x, ipd, sideBySide);
}

OSP_REGISTER_CAMERA(StereoCamera, stereoFull);

} // ::ospray
