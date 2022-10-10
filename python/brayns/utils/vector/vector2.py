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
class Vector2(Vector[float]):
    """2D vector with XY components.

    Provides dot product in addition to Vector operators.

    :param x: X component.
    :type x: float
    :param y: Y component.
    :type y: float
    """

    x: float = 0.0
    y: float = 0.0

    @staticmethod
    def full(value: float) -> Vector2:
        return Vector2(value, value)

    @classmethod
    @property
    def zero(cls) -> Vector2:
        return Vector2()

    @classmethod
    @property
    def one(cls) -> Vector2:
        return Vector2.full(1.0)

    def __iter__(self) -> Iterator[float]:
        yield self.x
        yield self.y

    def dot(self, other: Vector2) -> float:
        return sum(i * j for i, j in zip(self, other))
