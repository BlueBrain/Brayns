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

#include <brayns/engine/common/MathTypesOsprayTraits.h>

namespace
{
struct CameraParameters
{
    inline static const std::string position = "position";
    inline static const std::string direction = "direction";
    inline static const std::string up = "up";
    inline static const std::string aspect = "aspect";
};
}

namespace brayns
{
bool operator==(const LookAt &a, const LookAt &b) noexcept
{
    return a.position == b.position && a.target == b.target && a.up == b.up;
}

bool operator!=(const LookAt &a, const LookAt &b) noexcept
{
    return !(a == b);
}

Camera::Camera(const std::string &handleID)
    : _osprayCamera(handleID)
{
}

bool Camera::commit()
{
    if (!_flag)
    {
        return false;
    }

    const auto &position = _lookAtParams.position;
    const auto &target = _lookAtParams.target;
    const auto &up = _lookAtParams.up;
    const auto forward = glm::normalize(target - position);
    const auto strafe = glm::cross(forward, up);
    const auto realUp = glm::cross(strafe, forward);

    _osprayCamera.setParam(CameraParameters::position, position);
    _osprayCamera.setParam(CameraParameters::direction, forward);
    _osprayCamera.setParam(CameraParameters::up, realUp);
    _osprayCamera.setParam(CameraParameters::aspect, _aspectRatio);

    commitCameraSpecificParams();

    _osprayCamera.commit();

    _flag = false;

    return true;
}

void Camera::setLookAt(const LookAt &params) noexcept
{
    _flag.update(_lookAtParams, params);
}

const LookAt &Camera::getLookAt() const noexcept
{
    return _lookAtParams;
}

void Camera::setAspectRatio(const float aspectRatio) noexcept
{
    _flag.update(_aspectRatio, aspectRatio);
}

float Camera::getAspectRatio() const noexcept
{
    return _aspectRatio;
}

const ospray::cpp::Camera &Camera::getOsprayCamera() const noexcept
{
    return _osprayCamera;
}

ModifiedFlag &Camera::getModifiedFlag() noexcept
{
    return _flag;
}
} // namespace brayns
