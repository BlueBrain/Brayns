/* Copyright (c) 2015-2016, EPFL/Blue Brain Project
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

#include "Camera.h"
#include <brayns/common/log.h>

namespace brayns
{
Camera& Camera::operator=(const Camera& rhs)
{
    if (this == &rhs)
        return *this;

    clonePropertiesFrom(rhs);

    setPosition(rhs.getPosition());
    setTarget(rhs.getTarget());
    setUp(rhs.getUp());

    _initialPosition = rhs._initialPosition;
    _initialTarget = rhs._initialTarget;
    _initialUp = rhs._initialUp;

    _matrix = rhs._matrix;
    return *this;
}

void Camera::set(const Vector3d& position, const Vector3d& target,
                 const Vector3d& upVector)
{
    setPosition(position);
    setTarget(target);
    setUp(upVector);
}

void Camera::setInitialState(const Vector3d& position, const Vector3d& target,
                             const Vector3d& upVector)
{
    _initialPosition = position;
    _initialTarget = target;
    _initialUp = upVector;
    set(position, target, upVector);
}

void Camera::setInitialState(const Boxd& boundingBox)
{
    const auto& target = boundingBox.getCenter();
    const auto& diag = boundingBox.getSize();
    auto position = target;
    position.z() += diag.find_max();

    const Vector3d up(0., 1., 0.);
    setInitialState(position, target, up);

    BRAYNS_INFO << "World bounding box: " << boundingBox << std::endl;
    BRAYNS_INFO << "World center      : " << boundingBox.getCenter()
                << std::endl;
}

void Camera::reset()
{
    set(_initialPosition, _initialTarget, _initialUp);
    _matrix = Matrix4f();
}

std::ostream& operator<<(std::ostream& os, Camera& camera)
{
    const auto& position = camera.getPosition();
    const auto& target = camera.getTarget();
    const auto& up = camera.getUp();
    os << position.x() << "," << position.y() << "," << position.z() << ","
       << target.x() << "," << target.y() << "," << target.z() << "," << up.x()
       << "," << up.y() << "," << up.z();
    return os;
}
}
