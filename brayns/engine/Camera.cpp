/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *                     Jafet Villafranca <jafet.villafrancadiaz@epfl.ch>
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
Camera &Camera::operator=(const Camera &rhs)
{
    if (this == &rhs)
        return *this;

    clonePropertiesFrom(rhs);

    setPosition(rhs.getPosition());
    setOrientation(rhs.getOrientation());

    _initialPosition = rhs._initialPosition;
    _initialOrientation = rhs._initialOrientation;

    return *this;
}

void Camera::set(const Vector3d &position, const Quaterniond &orientation, const Vector3d &target)
{
    setPosition(position);
    setOrientation(orientation);
    setTarget(target);
}

void Camera::setInitialState(const Vector3d &position, const Quaterniond &orientation, const Vector3d &target)
{
    _initialPosition = position;
    _initialTarget = target;
    _initialOrientation = orientation;
    _initialOrientation = glm::normalize(_initialOrientation);
    set(position, orientation, target);
}

void Camera::setPosition(const Vector3d &position)
{
    _updateValue(_position, position);
}

void Camera::setTarget(const Vector3d &target)
{
    _updateValue(_target, target);
}

const Vector3d &Camera::getPosition() const
{
    return _position;
}

const Vector3d &Camera::getTarget() const
{
    return _target;
}

void Camera::setOrientation(Quaterniond orientation)
{
    orientation = glm::normalize(orientation);
    _updateValue(_orientation, orientation);
}

const Quaterniond &Camera::getOrientation() const
{
    return _orientation;
}

void Camera::reset()
{
    set(_initialPosition, _initialOrientation, _initialTarget);
}

void Camera::setBufferTarget(const std::string &target)
{
    _updateValue(_bufferTarget, target, false);
}

const std::string &Camera::getBufferTarget() const
{
    return _bufferTarget;
}
} // namespace brayns
