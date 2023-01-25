# Copyright (c) 2015-2023 EPFL/Blue Brain Project
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

from .vector import Vector, Vector3


class Quaternion(Vector[float]):
    """Quaternion with XYZW components.

    Provides vector operations (componentwise) and quaternion special operators
    (multiplication, conjugate, etc).

    :param x: X component, defaults to 0.
    :type x: float, optional
    :param y: Y component, defaults to 0.
    :type y: float, optional
    :param z: Z component, defaults to 0.
    :type z: float, optional
    :param w: W component, defaults to 1.
    :type w: float, optional
    """

    @classmethod
    @property
    def identity(cls) -> Quaternion:
        return Quaternion()

    def __new__(
        cls, x: float = 0.0, y: float = 0.0, z: float = 0.0, w: float = 1.0
    ) -> Quaternion:
        return super().__new__(cls, x, y, z, w)

    def __mul__(self, value: int | float | Quaternion) -> Quaternion:
        if isinstance(value, (int, float)):
            return self.unpack(i * value for i in self)
        x0, y0, z0, w0 = self
        x1, y1, z1, w1 = value
        return Quaternion(
            w0 * x1 + x0 * w1 + y0 * z1 - z0 * y1,
            w0 * y1 - x0 * z1 + y0 * w1 + z0 * x1,
            w0 * z1 + x0 * y1 - y0 * x1 + z0 * w1,
            w0 * w1 - x0 * x1 - y0 * y1 - z0 * z1,
        )

    def __rmul__(self, value: int | float | Quaternion) -> Quaternion:
        if isinstance(value, (int, float)):
            return self * value
        return value * self

    def __truediv__(self, value: int | float | Quaternion) -> Quaternion:
        if isinstance(value, (int, float)):
            return self.unpack(i / value for i in self)
        return self * value.inverse

    def __rtruediv__(self, value: int | float | Quaternion) -> Quaternion:
        if isinstance(value, (int, float)):
            return self.unpack(value / i for i in self)
        return value * self.inverse

    @property
    def x(self) -> float:
        return self[0]

    @property
    def y(self) -> float:
        return self[1]

    @property
    def z(self) -> float:
        return self[2]

    @property
    def w(self) -> float:
        return self[3]

    @property
    def xyz(self) -> Vector3:
        return Vector3(self.x, self.y, self.z)

    @property
    def axis(self) -> Vector3:
        return self.xyz.normalized

    @property
    def angle_radians(self) -> float:
        q = self.normalized
        return 2 * math.acos(q.w)

    @property
    def angle_degrees(self) -> float:
        return math.degrees(self.angle_radians)

    @property
    def conjugate(self) -> Quaternion:
        return Quaternion(-self.x, -self.y, -self.z, self.w)

    @property
    def inverse(self) -> Quaternion:
        return self.conjugate / self.square_norm
