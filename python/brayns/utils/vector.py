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
from collections.abc import Callable, Iterable
from typing import TypeVar

T = TypeVar("T", bound="Vector")
U = TypeVar("U", int, float)


class Vector(tuple[U, ...]):
    """Generic vector base class.

    Provide either componentwise (with another vector) and scalar operators (
    apply the scalar to all components).

    Examples of arithmetic operations with vector = [1, 1, 1]:

        vector + vector # 2 2 2
        vector * 3 # 3 3 3
        vector / 2 # 0.5 0.5 0.5
        2 / vector # 2 2 2
        vector = Vector3.unpack(i * i for i in range(3)) # 1 4 9
        vector = vector.normalized # vector.norm ~ 1
        vector.square_norm # 3
        vector.norm # sqrt(3)

    See below for supported operators.
    """

    @classmethod
    def unpack(cls: type[T], components: Iterable[U]) -> T:
        return cls(*components)

    def __new__(cls: type[T], *components: U) -> T:
        return super().__new__(cls, components)

    def __str__(self) -> str:
        name = type(self).__name__
        values = ", ".join(str(i) for i in self)
        return f"{name}({values})"

    def __repr__(self) -> str:
        return self.__str__()

    def __neg__(self: T) -> T:
        return self.unpack(-i for i in self)

    def __pos__(self: T) -> T:
        return self.unpack(+i for i in self)

    def __abs__(self: T) -> T:
        return self.unpack(abs(i) for i in self)

    def __add__(self: T, other: T) -> T:
        return self.unpack(i + j for i, j in zip(self, other))

    def __sub__(self: T, other: T) -> T:
        return self.unpack(i - j for i, j in zip(self, other))

    def __mul__(self: T, value: int | float | T) -> T:
        return self.__unpack(value, lambda x, y: x * y)

    def __rmul__(self: T, value: int | float | T) -> T:
        return self.__unpack(value, lambda x, y: y * x)

    def __truediv__(self: T, value: int | float | T) -> T:
        return self.__unpack(value, lambda x, y: x / y)

    def __rtruediv__(self: T, value: int | float | T) -> T:
        return self.__unpack(value, lambda x, y: y / x)

    def __floordiv__(self: T, value: int | float | T) -> T:
        return self.__unpack(value, lambda x, y: x // y)

    def __rfloordiv__(self: T, value: int | float | T) -> T:
        return self.__unpack(value, lambda x, y: y // x)

    def __mod__(self: T, value: int | float | T) -> T:
        return self.__unpack(value, lambda x, y: x % y)

    def __rmod__(self: T, value: int | float | T) -> T:
        return self.__unpack(value, lambda x, y: y % x)

    def __pow__(self: T, value: int | float | T) -> T:
        return self.__unpack(value, lambda x, y: x**y)

    def __rpow__(self: T, value: int | float | T) -> T:
        return self.__unpack(value, lambda x, y: y**x)

    @property
    def square_norm(self) -> float:
        return sum(i * i for i in self)

    @property
    def norm(self) -> float:
        return math.sqrt(self.square_norm)

    @property
    def normalized(self: T) -> T:
        return self / self.norm

    def dot(self: T, other: T) -> float:
        return sum(i * j for i, j in zip(self, other))

    def __unpack(
        self: T, value: int | float | T, operation: Callable[[float, float], float]
    ) -> T:
        if isinstance(value, (int, float)):
            return self.unpack(operation(i, value) for i in self)
        return self.unpack(operation(i, j) for i, j in zip(self, value))


class Vector2(Vector[float]):
    """2D vector with XY components.

    Provides dot product in addition to Vector operators.

    :param x: X component.
    :type x: float
    :param y: Y component.
    :type y: float
    """

    def __new__(cls, x: float = 0.0, y: float = 0.0) -> Vector2:
        return super().__new__(cls, x, y)

    @property
    def x(self) -> float:
        return self[0]

    @property
    def y(self) -> float:
        return self[1]


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
    def zero(cls) -> Vector3:
        return Vector3(0.0, 0.0, 0.0)

    @classmethod
    @property
    def one(cls) -> Vector3:
        return Vector3(1.0, 1.0, 1.0)

    def __new__(cls, x: float = 0.0, y: float = 0.0, z: float = 0.0) -> Vector3:
        return Vector[float].__new__(cls, x, y, z)

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
    def xy(self) -> Vector2:
        return Vector2(self.x, self.y)

    @property
    def xz(self) -> Vector2:
        return Vector2(self.x, self.z)

    @property
    def yz(self) -> Vector2:
        return Vector2(self.y, self.z)

    def cross(self, other: Vector3) -> Vector3:
        return Vector3(
            self.y * other.z - self.z * other.y,
            self.z * other.x - self.x * other.z,
            self.x * other.y - self.y * other.x,
        )


def componentwise_min(values: list[Vector3]) -> Vector3:
    """Return minimum of each component among values.

    If values is empty, zero is returned.

    :param values: List of vectors.
    :type values: list[Vector3]
    :return: Min value for each component.
    :rtype: Vector3
    """
    if not values:
        return Vector3.zero
    return Vector3(
        min(value.x for value in values),
        min(value.y for value in values),
        min(value.z for value in values),
    )


def componentwise_max(values: list[Vector3]) -> Vector3:
    """Return maximum of each component among values.

    If values is empty, zero is returned.

    :param values: List of vectors.
    :type values: list[Vector3]
    :return: Max value for each component.
    :rtype: Vector3
    """
    if not values:
        return Vector3.zero
    return Vector3(
        max(value.x for value in values),
        max(value.y for value in values),
        max(value.z for value in values),
    )


class Axis:
    """Helper class to store the principal axes of Brayns coordinate system."""

    @classmethod
    @property
    def x(cls) -> Vector3:
        return Vector3(1.0, 0.0, 0.0)

    @classmethod
    @property
    def right(cls) -> Vector3:
        return cls.x

    @classmethod
    @property
    def left(cls) -> Vector3:
        return -cls.right

    @classmethod
    @property
    def y(cls) -> Vector3:
        return Vector3(0.0, 1.0, 0.0)

    @classmethod
    @property
    def up(cls) -> Vector3:
        return cls.y

    @classmethod
    @property
    def down(cls) -> Vector3:
        return -cls.up

    @classmethod
    @property
    def z(cls) -> Vector3:
        return Vector3(0.0, 0.0, 1.0)

    @classmethod
    @property
    def front(cls) -> Vector3:
        return cls.z

    @classmethod
    @property
    def back(cls) -> Vector3:
        return -cls.front
