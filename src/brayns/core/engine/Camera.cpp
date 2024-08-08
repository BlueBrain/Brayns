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

void setCameraViewParams(OSPCamera handle, const CameraView &view)
{
    setObjectParam(handle, "position", view.position);
    setObjectParam(handle, "direction", view.direction);
    setObjectParam(handle, "up", view.up);
    setObjectParam(handle, "nearClip", view.nearClippingDistance);
}
}

namespace brayns
{
void Camera::setView(const CameraView &view)
{
    auto handle = getHandle();
    setCameraViewParams(handle, view);
    commitObject(handle);
}

void PerspectiveCamera::setFovy(float fovy)
{
    auto handle = getHandle();
    setObjectParam(handle, "fovy", fovy);
    commitObject(handle);
}

void PerspectiveCamera::setAspect(float aspect)
{
    auto handle = getHandle();
    setObjectParam(handle, "aspect", aspect);
    commitObject(handle);
}

PerspectiveCamera createPerspectiveCamera(Device &device, const CameraView &view, const Perspective &projection)
{
    auto handle = ospNewCamera("perspective");
    auto camera = wrapObjectHandleAs<PerspectiveCamera>(device, handle);

    setCameraViewParams(handle, view);

    setObjectParam(handle, "fovy", projection.fovy);
    setObjectParam(handle, "aspect", projection.aspect);

    commitObject(handle);

    return camera;
}

void OrthographicCamera::setHeight(float height)
{
    auto handle = getHandle();
    setObjectParam(handle, "height", height);
    commitObject(handle);
}

void OrthographicCamera::setAspect(float aspect)
{
    auto handle = getHandle();
    setObjectParam(handle, "aspect", aspect);
    commitObject(handle);
}

OrthographicCamera createOrthographicCamera(Device &device, const CameraView &view, const Orthographic &projection)
{
    auto handle = ospNewCamera("orthographic");
    auto camera = wrapObjectHandleAs<OrthographicCamera>(device, handle);

    setCameraViewParams(handle, view);

    setObjectParam(handle, "height", projection.height);
    setObjectParam(handle, "aspect", projection.aspect);

    commitObject(handle);

    return camera;
}
}
