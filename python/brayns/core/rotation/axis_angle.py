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

from brayns.core.vector.vector3 import Vector3
from brayns.core.rotation.quaternion import Quaternion


@dataclass
class AxisAngle:

    axis: Vector3
    angle: float = 0.0

    @staticmethod
    def from_quaternion(quaternion: Quaternion, degrees: bool = False) -> AxisAngle:
        axis = quaternion.vector
        angle = 2 * math.acos(quaternion.w)
        if degrees:
            angle = math.degrees(angle)
        return AxisAngle(axis, angle)

    def to_quaternion(self, degrees: bool = False) -> Quaternion:
        axis, angle = self.axis, self.angle
        if degrees:
            angle = math.radians(angle)
        half_angle = angle / 2
        vector = axis.normalized * math.sin(half_angle)
        real = math.cos(half_angle)
        return Quaternion(vector.x, vector.y, vector.z, real)
