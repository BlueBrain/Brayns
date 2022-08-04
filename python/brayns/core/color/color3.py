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

from collections.abc import Iterator
from dataclasses import dataclass
from typing import TypeVar

from brayns.core.vector.vector import Vector

T = TypeVar('T', bound='Color3')


@dataclass(frozen=True, order=True)
class Color3(Vector[float]):

    r: float = 0.0
    g: float = 0.0
    b: float = 0.0

    @classmethod
    @property
    def black(cls: type[T]) -> T:
        return cls()

    @classmethod
    @property
    def white(cls: type[T]) -> T:
        return cls(1.0, 1.0, 1.0)

    @classmethod
    @property
    def red(cls: type[T]) -> T:
        return cls(1.0, 0.0, 0.0)

    @classmethod
    @property
    def green(cls: type[T]) -> T:
        return cls(0.0, 1.0, 0.0)

    @classmethod
    @property
    def blue(cls: type[T]) -> T:
        return cls(0.0, 0.0, 1.0)

    @classmethod
    @property
    def bbp_background(cls: type[T]) -> T:
        return cls(0.004, 0.016, 0.102)

    def __iter__(self) -> Iterator[float]:
        yield self.r
        yield self.g
        yield self.b
