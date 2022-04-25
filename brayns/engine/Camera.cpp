/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *                     Jafet Villafranca <jafet.villafrancadiaz@epfl.ch>
 *                     Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

namespace brayns
{
bool operator==(const LookAt &a, const LookAt &b) noexcept
{
    return a.position == b.position && a.target == b.target && a.up == b.up;
}

Camera::~Camera()
{
    if (_handle)
    {
        ospRelease(_handle);
    }
}

bool Camera::commit()
{
    if (!isModified())
    {
        return false;
    }

    if (!_handle)
    {
        const auto handleName = getOSPHandleName();
        _handle = ospNewCamera(handleName.data());
    }

    const auto &position = _lookAtParams.position;
    const auto &target = _lookAtParams.target;
    const auto &up = _lookAtParams.up;

    const auto forward = glm::normalize(target - position);
    const auto strafe = glm::cross(forward, up);
    const auto realUp = glm::cross(strafe, forward);

    ospSetParam(_handle, "position", OSP_VEC3F, &position[0]);
    ospSetParam(_handle, "direction", OSP_VEC3F, &forward);
    ospSetParam(_handle, "up", OSP_VEC3F, &realUp);
    ospSetParam(_handle, "aspect", OSP_FLOAT, &_aspectRatio);

    commitCameraSpecificParams();

    ospCommit(_handle);

    resetModified();

    return true;
}

void Camera::setLookAt(const LookAt &params) noexcept
{
    _updateValue(_lookAtParams, params);
}

const LookAt &Camera::getLookAt() const noexcept
{
    return _lookAtParams;
}

void Camera::setAspectRatio(const float aspectRatio) noexcept
{
    _updateValue(_aspectRatio, aspectRatio);
}

float Camera::getAspectRatio() const noexcept
{
    return _aspectRatio;
}

OSPCamera Camera::handle() const noexcept
{
    return _handle;
}
} // namespace brayns
