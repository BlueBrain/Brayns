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
from abc import ABC, abstractmethod
from collections.abc import Callable, Iterable, Iterator
from typing import Generic, TypeVar

T = TypeVar('T', bound='Vector')
U = TypeVar('U', int, float)


class Vector(ABC, Generic[U]):
    """Generic vector base class.

    Provide either componentwise (with another vector) and scalar operators (
    apply the scalar to all components).

    Examples of arithmetic operations with vector = [1, 1, 1]:

        vector + vector # 2 2 2
        vector * 3 # 3 3 3
        vector / 2 # 0.5 0.5 0.5
        2 / vector # 2 2 2
        vector = Vector3.unpack(i*i for i in range(3)) # 1 4 9
        vector = vector.normalized # vector.norm ~ 1
        vector.square_norm # 3
        vector.norm # sqrt(3)

    See below for supported operators.
    """

    @classmethod
    @property
    @abstractmethod
    def component_count(cls) -> int:
        pass

    @classmethod
    def unpack(cls: type[T], components: Iterable[U]) -> T:
        return cls(*components)

    @classmethod
    def full(cls: type[T], value: U) -> T:
        return cls.unpack(value for _ in range(cls.component_count))

    def __init__(self, *components: U) -> None:
        if len(components) != self.component_count:
            raise ValueError(f'Invalid component count: {components}')
        self._components = components

    def __str__(self) -> str:
        return type(self).__name__ + self._components.__str__()

    def __repr__(self) -> str:
        return self.__str__()

    def __eq__(self, other: object) -> bool:
        if not isinstance(other, type(self)):
            return False
        return self._components == other._components

    def __lt__(self: T, other: T) -> bool:
        if not isinstance(other, type(self)):
            return NotImplemented
        return self._components < other._components

    def __le__(self: T, other: T) -> bool:
        if not isinstance(other, type(self)):
            return NotImplemented
        return self._components <= other._components

    def __gt__(self: T, other: T) -> bool:
        if not isinstance(other, type(self)):
            return NotImplemented
        return self._components > other._components

    def __ge__(self: T, other: T) -> bool:
        if not isinstance(other, type(self)):
            return NotImplemented
        return self._components >= other._components

    def __iter__(self) -> Iterator[U]:
        yield from self._components

    def __len__(self) -> int:
        return len(self._components)

    def __getitem__(self, index: int) -> U:
        return self._components[index]

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
        return self.__unpack(value, lambda x, y: x ** y)

    def __rpow__(self: T, value: int | float | T) -> T:
        return self.__unpack(value, lambda x, y: y ** x)

    @property
    def square_norm(self) -> float:
        return sum(i * i for i in self)

    @property
    def norm(self) -> float:
        return math.sqrt(self.square_norm)

    @property
    def normalized(self: T) -> T:
        return self / self.norm

    def __unpack(self: T, value: int | float | T, operation: Callable[[float, float], float]) -> T:
        if isinstance(value, (int, float)):
            return self.unpack(operation(i, value) for i in self)
        return self.unpack(operation(i, j) for i, j in zip(self, value))
