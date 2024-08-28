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
    setObjectParam(handle, "position", settings.view.position);
    setObjectParam(handle, "direction", settings.view.direction);
    setObjectParam(handle, "up", settings.view.up);
    setObjectParam(handle, "nearClip", settings.nearClip);
    setObjectParam(handle, "imageStart", settings.imageRegion.lower);
    setObjectParam(handle, "imageEnd", settings.imageRegion.upper);
}

void setStereoParams(OSPCamera handle, const std::optional<Stereo> &stereo)
{
    if (stereo)
    {
        setObjectParam(handle, "stereoMode", static_cast<OSPStereoMode>(stereo->mode));
        setObjectParam(handle, "interpupillaryDistance", stereo->interpupillaryDistance);
    }
    else
    {
        removeObjectParam(handle, "stereoMode");
        removeObjectParam(handle, "interpupillaryDistance");
    }
}

void setPerspectiveParams(OSPCamera handle, const PerspectiveSettings &settings)
{
    setObjectParam(handle, "fovy", settings.fovy);
    setObjectParam(handle, "aspect", settings.aspect);

    if (settings.depthOfField)
    {
        setObjectParam(handle, "apertureRadius", settings.depthOfField->apertureRadius);
        setObjectParam(handle, "focusDistance", settings.depthOfField->focusDistance);
    }
    else
    {
        removeObjectParam(handle, "apertureRadius");
        removeObjectParam(handle, "focusDistance");
    }

    setObjectParam(handle, "architectural", settings.architectural);

    setStereoParams(handle, settings.stereo);
}

void setOrthographicParams(OSPCamera handle, const OrthographicSettings &settings)
{
    setObjectParam(handle, "height", settings.height);
    setObjectParam(handle, "aspect", settings.aspect);
}

void setPanoramicParams(OSPCamera handle, const PanoramicSettings &settings)
{
    setStereoParams(handle, settings.stereo);
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

void PerspectiveCamera::update(const PerspectiveSettings &settings)
{
    auto handle = getHandle();
    setPerspectiveParams(handle, settings);
    commitObject(handle);
}

void PerspectiveCamera::setAspect(float aspect)
{
    auto handle = getHandle();
    setObjectParam(handle, "aspect", aspect);
    commitObject(handle);
}

PerspectiveCamera createPerspectiveCamera(
    Device &device,
    const CameraSettings &settings,
    const PerspectiveSettings &perspective)
{
    auto handle = ospNewCamera("perspective");
    auto camera = wrapObjectHandleAs<PerspectiveCamera>(device, handle);

    setCameraParams(handle, settings);
    setPerspectiveParams(handle, perspective);

    commitObject(device, handle);

    return camera;
}

void OrthographicCamera::update(const OrthographicSettings &settings)
{
    auto handle = getHandle();
    setOrthographicParams(handle, settings);
    commitObject(handle);
}

void OrthographicCamera::setAspect(float aspect)
{
    auto handle = getHandle();
    setObjectParam(handle, "aspect", aspect);
    commitObject(handle);
}

OrthographicCamera createOrthographicCamera(
    Device &device,
    const CameraSettings &settings,
    const OrthographicSettings &orthographic)
{
    auto handle = ospNewCamera("orthographic");
    auto camera = wrapObjectHandleAs<OrthographicCamera>(device, handle);

    setCameraParams(handle, settings);
    setOrthographicParams(handle, orthographic);

    commitObject(device, handle);

    return camera;
}

void PanoramicCamera::update(const PanoramicSettings &settings)
{
    auto handle = getHandle();
    setPanoramicParams(handle, settings);
    commitObject(handle);
}

PanoramicCamera createPanoramicCamera(Device &device, const CameraSettings &settings, const PanoramicSettings &panoramic)
{
    auto handle = ospNewCamera("panoramic");
    auto camera = wrapObjectHandleAs<PanoramicCamera>(device, handle);

    setCameraParams(handle, settings);
    setPanoramicParams(handle, panoramic);

    commitObject(device, handle);

    return camera;
}
}
