# Copyright (c) 2015-2022 EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
#
# Responsible Author: adrien.fleury@epfl.ch
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>
#
# This library is free software; you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License version 3.0 as published
# by the Free Software Foundation.
#
# This library is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
# details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this library; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

import math

from brayns.core.transform.quaternion import Quaternion
from brayns.core.vector.vector3 import Vector3


def quaternion_to_euler(quaternion: Quaternion, degrees: bool = False) -> Vector3:
    q = quaternion.normalized
    euler = Vector3(_get_x(q), _get_y(q), _get_z(q))
    if degrees:
        return Vector3.unpack(math.degrees(i) for i in euler)
    return euler


def _get_x(quaternion: Quaternion) -> float:
    q = quaternion
    sx_cy = 2 * (q.w * q.x + q.y * q.z)
    cx_cy = 1 - 2 * (q.x * q.x + q.y * q.y)
    return math.atan2(sx_cy, cx_cy)


def _get_y(quaternion: Quaternion) -> float:
    q = quaternion
    sy = 2 * (q.w * q.y - q.z * q.x)
    if abs(sy) >= 1:
        return math.copysign(math.pi / 2, sy)
    return math.asin(sy)


def _get_z(quaternion: Quaternion) -> float:
    q = quaternion
    sz_cy = 2 * (q.w * q.z + q.x * q.y)
    cz_cy = 1 - 2 * (q.y * q.y + q.z * q.z)
    return math.atan2(sz_cy, cz_cy)
