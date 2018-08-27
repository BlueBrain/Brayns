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

#include "CylindricStereoTrackedCamera.h"
#include "CylindricStereoTrackedCamera_ispc.h"

#define OPENDECK_HEIGHT 2.3f
#define OPENDECK_RADIUS 2.55f

namespace ospray
{
CylindricStereoTrackedCamera::CylindricStereoTrackedCamera()
{
    ispcEquivalent = ispc::CylindricStereoTrackedCamera_create(this);
}

std::string CylindricStereoTrackedCamera::toString() const
{
    return "ospray::CylindricStereoTrackedCamera";
}

void CylindricStereoTrackedCamera::commit()
{
    Camera::commit();

    const auto stereoMode =
        (StereoMode)getParam1i("stereoMode", OSP_STEREO_NONE);
    const auto interpupillaryDistance =
        getParamf("interpupillaryDistance", 0.0635f);
    const auto openDeckOrg =
        getParam3f("openDeckPosition", vec3f(0.0f, 0.0f, 0.0f));
    auto openDeckCamDU = getParam3f("openDeckCamDU", vec3f(1.0, 0.0f, 0.0f));

    dir = normalize(dir);
    openDeckCamDU = normalize(openDeckCamDU);

    auto dir_du = normalize(cross(dir, up));
    auto dir_dv = normalize(up);
    dir = -dir;

    const auto org = vec3f(0.0f, 0.0f, 0.0f);
    dir = vec3f(0, 0, 1);
    dir_du = vec3f(1, 0, 0);
    dir_dv = vec3f(0, 1, 0);

    float ipd = 0.0f;
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
        break;
    case OSP_STEREO_NONE:
        break;
    }
    const auto sideBySide = stereoMode == OSP_STEREO_SIDE_BY_SIDE;

    ispc::CylindricStereoTrackedCamera_set(
        getIE(), (const ispc::vec3f&)org, (const ispc::vec3f&)openDeckOrg,
        (const ispc::vec3f&)dir, (const ispc::vec3f&)dir_du,
        (const ispc::vec3f&)dir_dv, (const ispc::vec3f&)openDeckCamDU,
        OPENDECK_HEIGHT, OPENDECK_RADIUS, ipd, sideBySide);
}

OSP_REGISTER_CAMERA(CylindricStereoTrackedCamera, cylindricStereoTracked);
}
