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

from __future__ import annotations

import math
from dataclasses import dataclass

from brayns.core.common.quaternion import Quaternion
from brayns.core.vector.vector3 import Vector3


@dataclass(frozen=True, order=True)
class Euler(Vector3):

    @staticmethod
    def from_quaternion(quaternion: Quaternion, degrees: bool = False) -> Euler:
        return Euler(
            _get_x(quaternion, degrees),
            _get_y(quaternion, degrees),
            _get_z(quaternion, degrees)
        )

    def to_quaternion(self, degrees: bool = False) -> Quaternion:
        euler = self
        if degrees:
            euler = self.unpack(math.radians(i) for i in self)
        euler /= 2
        cx, cy, cz = self.unpack(math.cos(i) for i in euler)
        sx, sy, sz = self.unpack(math.sin(i) for i in euler)
        return Quaternion(
            sx * cy * cz - cx * sy * sz,
            cx * sy * cz + sx * cy * sz,
            cx * cy * sz - sx * sy * cz,
            cx * cy * cz + sx * sy * sz
        )


def _get_x(quaternion: Quaternion, degrees: bool = False) -> float:
    q = quaternion
    sx_cy = 2 * (q.w * q.x + q.y * q.z)
    cx_cy = 1 - 2 * (q.x * q.x + q.y * q.y)
    angle = math.atan2(sx_cy, cx_cy)
    return math.degrees(angle) if degrees else angle


def _get_y(quaternion: Quaternion, degrees: bool = False) -> float:
    q = quaternion
    sy = 2 * (q.w * q.y - q.z * q.x)
    angle = _safe_asin(sy)
    return math.degrees(angle) if degrees else angle


def _get_z(quaternion: Quaternion, degrees: bool = False) -> float:
    q = quaternion
    sz_cy = 2 * (q.w * q.z + q.x * q.y)
    cz_cy = 1 - 2 * (q.y * q.y + q.z * q.z)
    angle = math.atan2(sz_cy, cz_cy)
    return math.degrees(angle) if degrees else angle


def _safe_asin(value: float) -> float:
    if abs(value) >= 1:
        return math.copysign(math.pi / 2, value)
    return math.asin(value)
