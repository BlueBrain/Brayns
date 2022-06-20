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

#include "OsprayUtils.h"

namespace brayns
{
rkcommon::math::affine3f OsprayAffineConverter::fromTransform(const Transform &transform) noexcept
{
    auto matrix = transform.toMatrix();
    auto &strafe = matrix[0];
    auto &up = matrix[1];
    auto &forward = matrix[2];
    auto &position = matrix[3];

    rkcommon::math::affine3f result;
    result.l.vx = rkcommon::math::vec3f(strafe.x, strafe.y, strafe.z);
    result.l.vy = rkcommon::math::vec3f(up.x, up.y, up.z);
    result.l.vz = rkcommon::math::vec3f(forward.x, forward.y, forward.z);
    result.p = rkcommon::math::vec3f(position.x, position.y, position.z);

    return result;
}
}
