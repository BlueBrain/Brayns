/* Copyright 2018-2024 Blue Brain Project/EPFL
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

namespace
{
constexpr float OPENDECK_FOV_Y = 48.549f;
constexpr float DEFAULT_INTERPUPILLARY_DISTANCE = 0.0635f;
} // namespace

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

    const auto stereoMode = getStereoMode();
    const auto ipd = getInterpupillaryDistance(stereoMode);
    const auto sideBySide = stereoMode == StereoMode::OSP_STEREO_SIDE_BY_SIDE;

    dir = normalize(dir);
    // The tracking model of the 3d glasses is inversed
    // so we need to negate dir_du here.
    const auto dir_du = -normalize(cross(dir, up));
    const auto dir_dv = normalize(up);
    dir = -dir;

    const auto imgPlane_size_y = 2.0f * tanf(deg2rad(0.5f * OPENDECK_FOV_Y));

    ispc::CylindricStereoCamera_set(getIE(), (const ispc::vec3f&)pos,
                                    (const ispc::vec3f&)dir,
                                    (const ispc::vec3f&)dir_du,
                                    (const ispc::vec3f&)dir_dv, imgPlane_size_y,
                                    ipd, sideBySide);
}

CylindricStereoCamera::StereoMode CylindricStereoCamera::getStereoMode()
{
    return static_cast<StereoMode>(
        getParam1i("stereoMode", StereoMode::OSP_STEREO_SIDE_BY_SIDE));
}

float CylindricStereoCamera::getInterpupillaryDistance(
    const StereoMode stereoMode)
{
    const auto interpupillaryDistance =
        getParamf("interpupillaryDistance", DEFAULT_INTERPUPILLARY_DISTANCE);

    switch (stereoMode)
    {
    case StereoMode::OSP_STEREO_LEFT:
        return -interpupillaryDistance;
    case StereoMode::OSP_STEREO_RIGHT:
        return interpupillaryDistance;
    case StereoMode::OSP_STEREO_SIDE_BY_SIDE:
        return interpupillaryDistance;
    case StereoMode::OSP_STEREO_NONE:
    default:
        return 0.0f;
    }
}

OSP_REGISTER_CAMERA(CylindricStereoCamera, cylindricStereo);
} // namespace ospray
