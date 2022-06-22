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

#include "OrthographicCamera.h"

namespace
{
struct OrhtographicParameters
{
    inline static const std::string osprayName = "orthographic";
    inline static const std::string height = "height";
};
}

namespace brayns
{
OrthographicCamera::OrthographicCamera()
    : Camera(OrhtographicParameters::osprayName)
{
}

std::string OrthographicCamera::getName() const noexcept
{
    return "orthographic";
}

std::unique_ptr<Camera> OrthographicCamera::clone() const noexcept
{
    auto result = std::make_unique<OrthographicCamera>();
    result->setLookAt(getLookAt());
    result->setAspectRatio(getAspectRatio());
    result->setHeight(getHeight());
    return result;
}

void OrthographicCamera::setHeight(const float height) noexcept
{
    _updateValue(_height, height);
}

float OrthographicCamera::getHeight() const noexcept
{
    return _height;
}

void OrthographicCamera::commitCameraSpecificParams()
{
    auto &osprayCamera = getOsprayCamera();
    osprayCamera.setParam(OrhtographicParameters::height, _height);
}
}
