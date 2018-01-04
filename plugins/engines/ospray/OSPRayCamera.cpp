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

#include "OSPRayCamera.h"

#include <brayns/common/log.h>
#include <ospray/SDK/common/OSPCommon.h>

namespace brayns
{
OSPRayCamera::OSPRayCamera(const CameraType cameraType)
    : Camera(cameraType)
{
    std::string cameraAsString;
    switch (getType())
    {
    case CameraType::stereo:
        cameraAsString = "stereo";
        break;
    case CameraType::orthographic:
        cameraAsString = "orthographic";
        break;
    case CameraType::panoramic:
        cameraAsString = "panoramic";
        break;
    case CameraType::clipped:
        cameraAsString = "clippedperspective";
        break;
    default:
        cameraAsString = "perspective";
        break;
    }
    _camera = ospNewCamera(cameraAsString.c_str());
}

OSPRayCamera::~OSPRayCamera()
{
    ospRelease(_camera);
}

void OSPRayCamera::commit()
{
    const auto& position = getPosition();
    const auto& target = getTarget();
    const auto dir = normalize(target - position);
    const auto& up = getUp();

    ospSet3f(_camera, "pos", position.x(), position.y(), position.z());
    ospSet3f(_camera, "dir", dir.x(), dir.y(), dir.z());
    ospSet3f(_camera, "up", up.x(), up.y(), up.z());
    ospSet1f(_camera, "aspect", getAspectRatio());
    ospSet1f(_camera, "apertureRadius", getAperture());
    ospSet1f(_camera, "focusDistance", getFocalLength());
    ospSet1i(_camera, "stereoMode", static_cast<uint>(getStereoMode()));
    ospSet1f(_camera, "interpupillaryDistance", getEyeSeparation());
    ospSet1f(_camera, "fovy", getFieldOfView());
    ospSet1f(_camera, "architectural", true);

    // Clip planes
    const auto& clipPlanes = getClipPlanes();
    if (clipPlanes.size() == 6)
    {
        const std::string clipPlaneNames[6] = {"clipPlane1", "clipPlane2",
                                               "clipPlane3", "clipPlane4",
                                               "clipPlane5", "clipPlane6"};
        for (size_t i = 0; i < clipPlanes.size(); ++i)
        {
            const auto& clipPlane = clipPlanes[i];
            ospSet4f(_camera, clipPlaneNames[i].c_str(), clipPlane.x(),
                     clipPlane.y(), clipPlane.z(), clipPlane.w());
        }
    }
    ospCommit(_camera);
}

void OSPRayCamera::setEnvironmentMap(const bool)
{
}
}
