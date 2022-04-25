/* Copyright (c) 2015-2022 EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include "CameraAdapter.h"

namespace brayns
{
GenericLookAt::GenericLookAt(LookAt baseLookAt)
    : _lookAt(std::move(baseLookAt))
{
}

const Vector3f &GenericLookAt::getPosition() const noexcept
{
    return _lookAt.position;
}

void GenericLookAt::setPosition(const Vector3f &position) noexcept
{
    _lookAt.position = position;
}

const Vector3f &GenericLookAt::getTarget() const noexcept
{
    return _lookAt.target;
}

void GenericLookAt::setTarget(const Vector3f &target) noexcept
{
    _lookAt.target = target;
}

const Vector3f &GenericLookAt::getUp() const noexcept
{
    return _lookAt.up;
}

void GenericLookAt::setUp(const Vector3f &up) noexcept
{
    _lookAt.up = up;
}

const LookAt &GenericLookAt::getLookAt() const noexcept
{
    return _lookAt;
}
}
