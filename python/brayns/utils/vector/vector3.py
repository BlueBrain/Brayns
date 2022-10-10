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

from .vector2 import Vector2


class Vector3(Vector2):
    """3D vector with XYZ components.

    Provides dot and cross product in addition to Vector operators.

    :param x: X component.
    :type x: float
    :param y: Y component.
    :type y: float
    :param z: Z component.
    :type z: float
    """

    @staticmethod
    def from_vector2(value: Vector2, z: float = 0.0) -> Vector3:
        return Vector3(value.x, value.y, z)

    @classmethod
    @property
    def component_count(cls) -> int:
        return 3

    def __init__(self, x: float = 0.0, y: float = 0.0, z: float = 0.0) -> None:
        super(Vector2, self).__init__(x, y, z)

    @property
    def z(self) -> float:
        return self[2]

    @property
    def vector2(self) -> Vector2:
        return Vector2(self.x, self.y)

    def cross(self, other: Vector3) -> Vector3:
        return Vector3(
            self.y * other.z - self.z * other.y,
            self.z * other.x - self.x * other.z,
            self.x * other.y - self.y * other.x,
        )
