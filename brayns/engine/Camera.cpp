/* Copyright 2015-2024 Blue Brain Project/EPFL
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
Camera& Camera::operator=(const Camera& rhs)
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

void Camera::set(const Vector3d& position, const Quaterniond& orientation,
                 const Vector3d& target)
{
    setPosition(position);
    setOrientation(orientation);
    setTarget(target);
}

void Camera::setInitialState(const Vector3d& position,
                             const Quaterniond& orientation,
                             const Vector3d& target)
{
    _initialPosition = position;
    _initialTarget = target;
    _initialOrientation = orientation;
    _initialOrientation = glm::normalize(_initialOrientation);
    set(position, orientation, target);
}

void Camera::reset()
{
    set(_initialPosition, _initialOrientation, _initialTarget);
}
} // namespace brayns
