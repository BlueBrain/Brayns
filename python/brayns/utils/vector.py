# Copyright (c) 2015-2024 EPFL/Blue Brain Project
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
from collections.abc import Sequence
from typing import Callable, Generic, Iterable, Self, TypeVarTuple

Ts = TypeVarTuple("Ts")


class Vector(tuple[*Ts], Generic[*Ts]):
    @classmethod
    def component_count(cls) -> int:
        raise NotImplementedError()

    @classmethod
    def unpack(cls, components: Iterable[float]) -> Self:
        return cls(*components)  # type: ignore

    @classmethod
    def full(cls, value: float) -> Self:
        return cls.unpack(value for _ in range(cls.component_count()))

    @classmethod
    def zeros(cls) -> Self:
        return cls.full(0)

    @classmethod
    def ones(cls) -> Self:
        return cls.full(1)

    @classmethod
    def componentwise(cls, values: Sequence[Self], operation: Callable[[Iterable[float]], float]) -> Self:
        return cls.unpack(operation(value[i] for value in values) for i in range(cls.component_count()))  # type: ignore

    @classmethod
    def componentwise_min(cls, values: Sequence[Self]) -> Self:
        return cls.componentwise(values, min)

    @classmethod
    def componentwise_max(cls, values: Sequence[Self]) -> Self:
        return cls.componentwise(values, max)

    def __new__(cls, *args: *Ts) -> Self:
        return super().__new__(cls, args)

    def __neg__(self) -> Self:
        return self.unpack(-i for i in self)  # type: ignore

    def __pos__(self) -> Self:
        return self.unpack(+i for i in self)  # type: ignore

    def __abs__(self) -> Self:
        return self.unpack(abs(i) for i in self)  # type: ignore

    def __add__(self, value: int | float | Self) -> Self:  # type: ignore
        return self._apply(value, lambda x, y: x + y)

    def __radd__(self, value: int | float | Self) -> Self:  # type: ignore
        return self._apply(value, lambda x, y: y + x)

    def __sub__(self, value: int | float | Self) -> Self:
        return self._apply(value, lambda x, y: x - y)

    def __rsub__(self, value: int | float | Self) -> Self:
        return self._apply(value, lambda x, y: y - x)

    def __mul__(self, value: int | float | Self) -> Self:  # type: ignore
        return self._apply(value, lambda x, y: x * y)

    def __rmul__(self, value: int | float | Self) -> Self:  # type: ignore
        return self._apply(value, lambda x, y: y * x)

    def __truediv__(self, value: int | float | Self) -> Self:
        return self._apply(value, lambda x, y: x / y)

    def __rtruediv__(self, value: int | float | Self) -> Self:
        return self._apply(value, lambda x, y: y / x)

    def __floordiv__(self, value: int | float | Self) -> Self:
        return self._apply(value, lambda x, y: x // y)

    def __rfloordiv__(self, value: int | float | Self) -> Self:
        return self._apply(value, lambda x, y: y // x)

    def __mod__(self, value: int | float | Self) -> Self:
        return self._apply(value, lambda x, y: x % y)

    def __rmod__(self, value: int | float | Self) -> Self:
        return self._apply(value, lambda x, y: y % x)

    def __pow__(self, value: int | float | Self) -> Self:
        return self._apply(value, lambda x, y: x**y)

    def __rpow__(self, value: int | float | Self) -> Self:
        return self._apply(value, lambda x, y: y**x)

    @property
    def square_norm(self) -> float:
        return sum(i * i for i in self)  # type: ignore

    @property
    def norm(self) -> float:
        return math.sqrt(self.square_norm)

    @property
    def normalized(self) -> Self:
        return self / self.norm

    def dot(self, other: Self) -> float:
        return sum(i * j for i, j in zip(self, other))  # type: ignore

    def reduce(self, operation: Callable[[float, float], float]) -> float:
        value = self[0]  # type: ignore

        for i in range(1, self.component_count()):
            value = operation(value, self[i])  # type: ignore

        return value  # type: ignore

    def reduce_multiply(self) -> float:
        return self.reduce(lambda x, y: x * y)

    def _apply(self, value, operation) -> Self:
        if isinstance(value, (int, float)):
            return self.unpack(operation(i, value) for i in self)

        return self.unpack(operation(i, j) for i, j in zip(self, value))  # type: ignore


class Vector2(Vector[float, float]):
    @classmethod
    def component_count(cls) -> int:
        return 2

    def __new__(cls, x: float = 0.0, y: float = 0.0) -> Self:
        return super().__new__(cls, x, y)

    @property
    def x(self) -> float:
        return self[0]

    @property
    def y(self) -> float:
        return self[1]


class Vector3(Vector[float, float, float]):
    @classmethod
    def component_count(cls) -> int:
        return 3

    def __new__(cls, x: float = 0.0, y: float = 0.0, z: float = 0.0) -> Self:
        return super().__new__(cls, x, y, z)

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

    def cross(self, other: Self) -> Self:
        return type(self)(
            self.y * other.z - self.z * other.y,
            self.z * other.x - self.x * other.z,
            self.x * other.y - self.y * other.x,
        )


class Vector4(Vector[float, float, float, float]):
    @classmethod
    def component_count(cls) -> int:
        return 4

    def __new__(cls, x: float = 0.0, y: float = 0.0, z: float = 0.0, w: float = 0.0) -> Self:
        return super().__new__(cls, x, y, z, w)

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
