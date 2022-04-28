/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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

#include <brayns/engine/cameras/PerspectiveCamera.h>

namespace brayns
{
PerspectiveCamera::PerspectiveCamera()
    : Camera("perspective")
{
}

std::string PerspectiveCamera::getName() const noexcept
{
    return "perspective";
}

std::unique_ptr<Camera> PerspectiveCamera::clone() const noexcept
{
    auto result = std::make_unique<PerspectiveCamera>();
    result->setApertureRadius(getApertureRadius());
    result->setAspectRatio(getAspectRatio());
    result->setFocusDistance(getFocusDistance());
    result->setFOVY(getFOVY());
    result->setLookAt(getLookAt());
    return result;
}

void PerspectiveCamera::commitCameraSpecificParams()
{
    auto ospHandle = handle();
    ospSetParam(ospHandle, "fovy", OSP_FLOAT, &_fovy);
    ospSetParam(ospHandle, "apertureRadius", OSP_FLOAT, &_apertureRadius);
    ospSetParam(ospHandle, "focusDistance", OSP_FLOAT, &_focusDistance);
}

void PerspectiveCamera::setFOVY(const float fovy) noexcept
{
    _updateValue(_fovy, fovy);
}

void PerspectiveCamera::setApertureRadius(const float aperture) noexcept
{
    _updateValue(_apertureRadius, aperture);
}

void PerspectiveCamera::setFocusDistance(const float distance) noexcept
{
    _updateValue(_focusDistance, distance);
}

float PerspectiveCamera::getFOVY() const noexcept
{
    return _fovy;
}

float PerspectiveCamera::getApertureRadius() const noexcept
{
    return _apertureRadius;
}

float PerspectiveCamera::getFocusDistance() const noexcept
{
    return _focusDistance;
}
}
