/* Copyright (c) 2018, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Grigori Chevtchenko <grigori.chevtchenko@epfl.ch>
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

#include "CylindricStereoCamera.h"
#include "CylindricStereoCamera_ispc.h"

#define OPENDECK_FOV_Y 48.549f

namespace ospray
{

CylindricStereoCamera::CylindricStereoCamera()
{
    ispcEquivalent = ispc::CylindricStereoCamera_create(this);
}

std::string CylindricStereoCamera::toString() const
{
    return "ospray::CylindricStereoCamera";
}

void CylindricStereoCamera::commit()
{
    Camera::commit();

    const float fovY = OPENDECK_FOV_Y;
    const StereoMode stereoMode = (StereoMode)getParam1i("stereoMode", OSP_STEREO_SIDE_BY_SIDE);
    const float interpupillaryDistance = getParamf("interpupillaryDistance", 0.0635f);

    dir = normalize(dir);
    const vec3f dir_du = normalize(cross(dir, up));
    const vec3f dir_dv = normalize(up);
    dir = -dir;

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

    const vec3f org = pos;
    const float imgPlane_size_y = 2.0f * tanf(deg2rad(0.5f * fovY));

    ispc::CylindricStereoCamera_set(getIE(), (const ispc::vec3f&)org,
                                             (const ispc::vec3f&)dir,
                                             (const ispc::vec3f&)dir_du,
                                             (const ispc::vec3f&)dir_dv,
                                             imgPlane_size_y, ipd,
                                             sideBySide);
}

OSP_REGISTER_CAMERA(CylindricStereoCamera, cylindricStereo);

} // ::ospray
