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
from abc import ABC, abstractmethod
from typing import Callable, Iterable, Iterator, TypeVar, Union

T = TypeVar('T', bound='Vector')


class Vector(ABC):

    @abstractmethod
    def __iter__(self) -> Iterator[float]:
        pass

    @classmethod
    def unpack(cls: type[T], values: Iterable[float]) -> T:
        return cls(*values)

    def __neg__(self: T) -> T:
        return self.unpack(-i for i in self)

    def __abs__(self: T) -> T:
        return self.unpack(abs(i) for i in self)

    def __add__(self: T, other: T) -> T:
        return self.unpack(i + j for i, j in zip(self, other))

    def __sub__(self: T, other: T) -> T:
        return self.unpack(i - j for i, j in zip(self, other))

    def __mul__(self: T, value: Union[int, float, T]) -> T:
        return self.__unpack(value, lambda x, y: x * y)

    def __rmul__(self: T, value: Union[int, float, T]) -> T:
        return self.__unpack(value, lambda x, y: y * x)

    def __truediv__(self: T, value: Union[int, float, T]) -> T:
        return self.__unpack(value, lambda x, y: x / y)

    def __rtruediv__(self: T, value: Union[int, float, T]) -> T:
        return self.__unpack(value, lambda x, y: y / x)

    def __floordiv__(self: T, value: Union[int, float, T]) -> T:
        return self.__unpack(value, lambda x, y: x // y)

    def __rfloordiv__(self: T, value: Union[int, float, T]) -> T:
        return self.__unpack(value, lambda x, y: y // x)

    def __mod__(self: T, value: Union[int, float, T]) -> T:
        return self.__unpack(value, lambda x, y: x % y)

    def __rmod__(self: T, value: Union[int, float, T]) -> T:
        return self.__unpack(value, lambda x, y: y % x)

    def __pow__(self: T, value: Union[int, float, T]) -> T:
        return self.__unpack(value, lambda x, y: x ** y)

    def __rpow__(self: T, value: Union[int, float, T]) -> T:
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

    def __unpack(self: T, value: Union[int, float, T], operation: Callable[[float, float], float]) -> T:
        if isinstance(value, (int, float)):
            return self.unpack(operation(i, value) for i in self)
        return self.unpack(operation(i, j) for i, j in zip(self, value))
