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

#include <brayns/engine/ospray/OsprayMathtypesTraits.h>

namespace
{
class CameraParameterUpdater
{
public:
    static void updateView(const brayns::Camera &camera)
    {
        static const std::string positionParam = "position";
        static const std::string directionParam = "direction";
        static const std::string upParam = "up";

        const auto &lookAt = camera.getLookAt();
        const auto &position = lookAt.position;
        const auto &target = lookAt.target;
        const auto &up = lookAt.up;
        const auto forward = glm::normalize(target - position);
        const auto strafe = glm::cross(forward, up);
        const auto realUp = glm::cross(strafe, forward);

        const auto &osprayCamera = camera.getOsprayCamera();
        osprayCamera.setParam(positionParam, position);
        osprayCamera.setParam(directionParam, forward);
        osprayCamera.setParam(upParam, realUp);
    }

    static void updateAspectRatio(const brayns::Camera &camera)
    {
        static const std::string aspectParam = "aspect";

        const auto aspectRatio = camera.getAspectRatio();
        const auto &osprayCamera = camera.getOsprayCamera();
        osprayCamera.setParam(aspectParam, aspectRatio);
    }
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
    if (!isModified())
    {
        return false;
    }

    CameraParameterUpdater::updateView(*this);
    CameraParameterUpdater::updateAspectRatio(*this);

    commitCameraSpecificParams();

    _osprayCamera.commit();

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

const ospray::cpp::Camera &Camera::getOsprayCamera() const noexcept
{
    return _osprayCamera;
}
} // namespace brayns
