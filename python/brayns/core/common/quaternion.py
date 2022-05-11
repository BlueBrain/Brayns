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
from dataclasses import dataclass
from typing import Iterator, Union

from brayns.core.common.vector import Vector
from brayns.core.common.vector3 import Vector3


@dataclass(frozen=True, order=True)
class Quaternion(Vector):

    x: float = 0.0
    y: float = 0.0
    z: float = 0.0
    w: float = 1.0

    @staticmethod
    def from_euler(euler: Vector3, degrees: bool = False) -> 'Quaternion':
        if degrees:
            euler = Vector3.unpack(math.radians(i) for i in euler)
        euler /= 2
        cx, cy, cz = Vector3.unpack(math.cos(i) for i in euler)
        sx, sy, sz = Vector3.unpack(math.sin(i) for i in euler)
        return Quaternion(
            sx * cy * cz - cx * sy * sz,
            cx * sy * cz + sx * cy * sz,
            cx * cy * sz - sx * sy * cz,
            cx * cy * cz + sx * sy * sz
        )

    @staticmethod
    def from_axis_angle(axis: Vector3, angle: float, degrees=False) -> 'Quaternion':
        if degrees:
            angle = math.radians(angle)
        half_angle = angle / 2
        axis = axis.normalized * math.sin(half_angle)
        return Quaternion(*axis, math.cos(half_angle))

    @staticmethod
    def from_vector(value: Vector3) -> 'Quaternion':
        return Quaternion(*value, 0.0)

    @classmethod
    @property
    def identity(cls) -> 'Quaternion':
        return Quaternion()

    def __iter__(self) -> Iterator[float]:
        yield self.x
        yield self.y
        yield self.z
        yield self.w

    def __mul__(self, value: Union[int, float, 'Quaternion']) -> 'Quaternion':
        if isinstance(value, (int, float)):
            return Quaternion.unpack(i * value for i in self)
        x0, y0, z0, w0 = self
        x1, y1, z1, w1 = value
        return Quaternion(
            w0 * x1 + x0 * w1 + y0 * z1 - z0 * y1,
            w0 * y1 - x0 * z1 + y0 * w1 + z0 * x1,
            w0 * z1 + x0 * y1 - y0 * x1 + z0 * w1,
            w0 * w1 - x0 * x1 - y0 * y1 - z0 * z1
        )

    def __rmul__(self, value: Union[int, float, 'Quaternion']) -> 'Quaternion':
        if isinstance(value, (int, float)):
            return self * value
        return value * self

    def __truediv__(self, value: Union[int, float, 'Quaternion']) -> 'Quaternion':
        if isinstance(value, (int, float)):
            return Quaternion.unpack(i / value for i in self)
        return self * value.inverse

    def __rtruediv__(self, value: Union[int, float, 'Quaternion']) -> 'Quaternion':
        if isinstance(value, (int, float)):
            return Quaternion.unpack(value / i for i in self)
        return value * self.inverse

    @property
    def vector(self) -> Vector3:
        return Vector3(self.x, self.y, self.z)

    @property
    def conjugate(self) -> 'Quaternion':
        return Quaternion(-self.x, -self.y, -self.z, self.w)

    @property
    def inverse(self) -> 'Quaternion':
        return self.conjugate / self.square_norm

    def rotate(self, value: Vector3, center=Vector3.zero) -> Vector3:
        rotation = self.normalized
        quaternion = Quaternion.from_vector(value - center)
        quaternion = rotation * quaternion * rotation.conjugate
        return center + quaternion.vector
