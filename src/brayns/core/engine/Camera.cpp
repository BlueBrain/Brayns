/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 *
 * Responsible Author: adrien.fleury@epfl.ch
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

#include "Camera.h"

namespace
{
using namespace brayns;

void setCameraParams(OSPCamera handle, const CameraSettings &settings)
{
    setObjectParam(handle, "position", settings.position);
    setObjectParam(handle, "direction", settings.direction);
    setObjectParam(handle, "up", settings.up);
    setObjectParam(handle, "nearClip", settings.nearClip);
    setObjectParam(handle, "imageStart", settings.imageRegion.lower);
    setObjectParam(handle, "imageEnd", settings.imageRegion.upper);
}

void setPerspectiveParams(OSPCamera handle, const PerspectiveCameraSettings &settings)
{
    setObjectParam(handle, "fovy", degrees(settings.fovy));
    setObjectParam(handle, "aspect", settings.aspect);
    setObjectParam(handle, "apertureRadius", settings.apertureRadius);
    setObjectParam(handle, "focusDistance", settings.focusDistance);
    setObjectParam(handle, "architectural", settings.architectural);
    setObjectParam(handle, "stereoMode", static_cast<OSPStereoMode>(settings.stereo));
    setObjectParam(handle, "interpupillaryDistance", settings.interpupillaryDistance);
}

void setOrthographicParams(OSPCamera handle, const OrthographicCameraSettings &settings)
{
    setObjectParam(handle, "height", settings.height);
    setObjectParam(handle, "aspect", settings.aspect);
}

void setPanoramicParams(OSPCamera handle, const PanoramicCameraSettings &settings)
{
    setObjectParam(handle, "stereoMode", static_cast<OSPStereoMode>(settings.stereo));
    setObjectParam(handle, "interpupillaryDistance", settings.interpupillaryDistance);
}
}

namespace brayns
{
void Camera::update(const CameraSettings &settings)
{
    auto handle = getHandle();
    setCameraParams(handle, settings);
    commitObject(handle);
}

void PerspectiveCamera::update(const PerspectiveCameraSettings &settings)
{
    auto handle = getHandle();
    setPerspectiveParams(handle, settings);
    commitObject(handle);
}

PerspectiveCamera createPerspectiveCamera(Device &device, const CameraSettings &settings, const PerspectiveCameraSettings &perspective)
{
    auto handle = ospNewCamera("perspective");
    auto camera = wrapObjectHandleAs<PerspectiveCamera>(device, handle);

    setCameraParams(handle, settings);
    setPerspectiveParams(handle, perspective);

    commitObject(device, handle);

    return camera;
}

void OrthographicCamera::update(const OrthographicCameraSettings &settings)
{
    auto handle = getHandle();
    setOrthographicParams(handle, settings);
    commitObject(handle);
}

OrthographicCamera createOrthographicCamera(Device &device, const CameraSettings &settings, const OrthographicCameraSettings &orthographic)
{
    auto handle = ospNewCamera("orthographic");
    auto camera = wrapObjectHandleAs<OrthographicCamera>(device, handle);

    setCameraParams(handle, settings);
    setOrthographicParams(handle, orthographic);

    commitObject(device, handle);

    return camera;
}

void PanoramicCamera::update(const PanoramicCameraSettings &settings)
{
    auto handle = getHandle();
    setPanoramicParams(handle, settings);
    commitObject(handle);
}

PanoramicCamera createPanoramicCamera(Device &device, const CameraSettings &settings, const PanoramicCameraSettings &panoramic)
{
    auto handle = ospNewCamera("panoramic");
    auto camera = wrapObjectHandleAs<PanoramicCamera>(device, handle);

    setCameraParams(handle, settings);
    setPanoramicParams(handle, panoramic);

    commitObject(device, handle);

    return camera;
}
}
