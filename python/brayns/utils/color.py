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

from typing import Self, TypeVarTuple

from .vector import Vector

Ts = TypeVarTuple("Ts")


class Color3(Vector[float, float, float]):
    def __new__(cls, r: float = 0.0, g: float = 0.0, b: float = 0.0) -> Self:
        return super().__new__(cls, r, g, b)

    @property
    def r(self) -> float:
        return self[0]

    @property
    def g(self) -> float:
        return self[1]

    @property
    def b(self) -> float:
        return self[2]


class Color4(Vector[float, float, float, float]):
    def __new__(cls, r: float = 0.0, g: float = 0.0, b: float = 0.0, a: float = 1.0) -> Self:
        return super().__new__(cls, r, g, b, a)

    @property
    def r(self) -> float:
        return self[0]

    @property
    def g(self) -> float:
        return self[1]

    @property
    def b(self) -> float:
        return self[2]

    @property
    def a(self) -> float:
        return self[3]

    @property
    def rgb(self) -> Color3:
        return Color3(self.r, self.g, self.b)
