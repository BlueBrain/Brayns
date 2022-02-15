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

#include <brayns/engine/Camera.h>

namespace brayns
{
Camera::Camera(const Camera& o) noexcept
{
    *this = o;
}

Camera &Camera::operator=(const Camera &o) noexcept
{
    _position = o._position;
    _target = o._target;
    _up = o._up;
    _aspectRatio = o._aspectRatio;

    markModified();

    return *this;
}

Camera::~Camera()
{
    if(_handle)
        ospRelease(_handle);
}

void Camera::commit()
{
    if(!_handle)
        throw std::runtime_error("Camera handle not initialized");

    const auto direction = glm::normalize(_target - _position);

    // TODO RECOMPUTE STRAFE AND UP TO MAKE SURE THEY ARE CORRECT

    ospSetParam(_handle, "position", OSP_VEC3F, &_position[0]);
    ospSetParam(_handle, "direction", OSP_VEC3F, &direction[0]);
    ospSetParam(_handle, "up", OSP_VEC3F, &_up[0]);
    ospSetParam(_handle, "aspect", OSP_FLOAT, &_aspectRatio);

    commitCameraSpecificParams();

    ospCommit(_handle);
}

void Camera::setPosition(const Vector3f &position) noexcept
{
    _updateValue(_position, position);
}

void Camera::setTarget(const Vector3f &target) noexcept
{
    _updateValue(_target, target);
}

void Camera::setUp(const Vector3f &up) noexcept
{
    _updateValue(_up, glm::normalize(up));
}

void Camera::setAspectRatio(const float aspectRatio) noexcept
{
    _aspectRatio = aspectRatio;
}

const Vector3f& Camera::getPosition() const noexcept
{
    return _position;
}

const Vector3f& Camera::getTarget() const noexcept
{
    return _target;
}

const Vector3f& Camera::getUp() const noexcept
{
    return _up;
}

OSPCamera Camera::handle() const noexcept
{
    return _handle;
}
} // namespace brayns
