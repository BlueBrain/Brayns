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

#include "CylindricCamera.h"

namespace
{
struct CylindricParameters
{
    inline static const std::string fovy = "fovy";
};
}

CylindricCamera::CylindricCamera()
    : brayns::Camera(typeName)
{
}

std::string CylindricCamera::getName() const noexcept
{
    return typeName;
}

std::unique_ptr<brayns::Camera> CylindricCamera::clone() const noexcept
{
    auto result = std::make_unique<CylindricCamera>();
    result->setLookAt(getLookAt());
    result->setAspectRatio(getAspectRatio());
    result->setFovy(_fovy);
    return result;
}

void CylindricCamera::setFovy(float fovy) noexcept
{
    getModifiedFlag().update(_fovy, fovy);
}

float CylindricCamera::getFovy() const noexcept
{
    return _fovy;
}

void CylindricCamera::commitCameraSpecificParams()
{
    auto &osprayCamera = getOsprayCamera();
    osprayCamera.setParam(CylindricParameters::fovy, _fovy);
}
