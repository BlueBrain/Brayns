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

namespace ospray
{
namespace
{
const vec3f OPENDECK_RIGHT_DIRECTION{1.0f, 0.0f, 0.0f};

vec3f _rotateVectorByQuat(const vec3f& v, const vec4f& q)
{
    const auto u = vec3f{q[0], q[1], q[2]}; // vector part of the quaternion
    const auto s = q[3];                    // scalar part of the quaternion

    return 2.0f * dot(u, v) * u + (s * s - dot(u, u)) * v +
           2.0f * s * cross(u, v);
}
}

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

    const auto stereoMode = getStereoMode();
    const auto ipd = getInterpupillaryDistance(stereoMode);
    const auto sideBySide = stereoMode == StereoMode::OSP_STEREO_SIDE_BY_SIDE;

    const auto headPosition = _getHeadPosition();
    const auto openDeckCamDU = _getOpendeckCamDU();

    dir = vec3f(0, 0, 1);
    const auto org = vec3f(0, 0, 0);
    const auto dir_du = vec3f(1, 0, 0);
    const auto dir_dv = vec3f(0, 1, 0);

    ispc::CylindricStereoTrackedCamera_set(getIE(), (const ispc::vec3f&)org,
                                           (const ispc::vec3f&)headPosition,
                                           (const ispc::vec3f&)dir,
                                           (const ispc::vec3f&)dir_du,
                                           (const ispc::vec3f&)dir_dv,
                                           (const ispc::vec3f&)openDeckCamDU,
                                           ipd, sideBySide);
}

vec3f CylindricStereoTrackedCamera::_getHeadPosition()
{
    return getParam3f("headPosition", vec3f(0.0f, 0.0f, 0.0f));
}

vec3f CylindricStereoTrackedCamera::_getOpendeckCamDU()
{
    const auto quat = getParam4f("headRotation", vec4f(0.0f, 0.0f, 0.0f, 1.0f));
    const auto cameraDU = _rotateVectorByQuat(OPENDECK_RIGHT_DIRECTION, quat);
    return normalize(cameraDU);
}

OSP_REGISTER_CAMERA(CylindricStereoTrackedCamera, cylindricStereoTracked);
}
