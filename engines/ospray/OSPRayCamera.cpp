/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#include <brayns/engine/Scene.h>

#include "OSPRayCamera.h"
#include "utils.h"

namespace brayns
{
OSPRayCamera::~OSPRayCamera()
{
    ospRelease(_camera);
}

void OSPRayCamera::commit()
{
    if (!isModified())
        return;

    const bool cameraChanged = _currentOSPCamera != getCurrentType();
    if (cameraChanged)
        _createOSPCamera();

    const auto position = getPosition();
    const auto dir = glm::rotate(getOrientation(), Vector3d(0., 0., -1.));
    const auto up = glm::rotate(getOrientation(), Vector3d(0., 1., 0.));

    osphelper::set(_camera, "pos", Vector3f(position));
    osphelper::set(_camera, "dir", Vector3f(dir));
    osphelper::set(_camera, "up", Vector3f(up));
    osphelper::set(_camera, "buffer_target", getBufferTarget());

    toOSPRayProperties(*this, _camera);

    // Clip planes
    if (!_clipPlanes.empty())
    {
        const auto clipPlanes = convertVectorToFloat(_clipPlanes);
        auto clipPlaneData =
            ospNewData(clipPlanes.size(), OSP_FLOAT4, clipPlanes.data());
        ospSetData(_camera, "clipPlanes", clipPlaneData);
        ospRelease(clipPlaneData);
    }
    else
    {
        // ospRemoveParam leaks objects, so we set it to null first
        ospSetData(_camera, "clipPlanes", nullptr);
        ospRemoveParam(_camera, "clipPlanes");
    }

    ospCommit(_camera);
}

void OSPRayCamera::setClipPlanes(const std::vector<Plane>& planes)
{
    if (_clipPlanes == planes)
        return;
    _clipPlanes = planes;
    markModified(false);
}

void OSPRayCamera::_createOSPCamera()
{
    auto newCamera = ospNewCamera(getCurrentType().c_str());
    if (!newCamera)
        throw std::runtime_error(getCurrentType() +
                                 " is not a registered camera");
    if (_camera)
        ospRelease(_camera);
    _camera = newCamera;
    _currentOSPCamera = getCurrentType();
    markModified(false);
}
} // namespace brayns
