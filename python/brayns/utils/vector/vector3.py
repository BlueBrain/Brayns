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

from collections.abc import Iterator
from dataclasses import dataclass

from .vector import Vector


@dataclass(frozen=True, order=True)
class Vector3(Vector[float]):
    """3D vector with XYZ components.

    Provides scalar and dot product in addition to Vector operators.

    Also gives the basic space directions (up, down, front, etc...).

    :param x: X component.
    :type x: float
    :param y: Y component.
    :type y: float
    :param z: Z component.
    :type z: float
    """

    x: float = 0.0
    y: float = 0.0
    z: float = 0.0

    @staticmethod
    def full(value: float) -> Vector3:
        return Vector3(value, value, value)

    @classmethod
    @property
    def zero(cls) -> Vector3:
        return Vector3()

    @classmethod
    @property
    def one(cls) -> Vector3:
        return Vector3.full(1.0)

    def __iter__(self) -> Iterator[float]:
        yield self.x
        yield self.y
        yield self.z

    def dot(self, other: Vector3) -> float:
        return sum(i * j for i, j in zip(self, other))

    def cross(self, other: Vector3) -> Vector3:
        return Vector3(
            self.y * other.z - self.z * other.y,
            self.z * other.x - self.x * other.z,
            self.x * other.y - self.y * other.x,
        )
