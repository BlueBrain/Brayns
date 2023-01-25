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

from .vector import Vector, Vector3


class PlaneEquation(Vector[float]):
    """Plane equation satisfying ax + by + cz + d = 0.

    Normal and direction point to the upper side of the plane.

    :param a: A component.
    :type a: float
    :param b: B component.
    :type b: float
    :param c: C component.
    :type c: float
    :param d: D component.
    :type d: float
    """

    def __new__(cls, a: float, b: float, c: float, d: float = 0.0) -> PlaneEquation:
        return super().__new__(cls, a, b, c, d)

    @property
    def a(self) -> float:
        return self[0]

    @property
    def b(self) -> float:
        return self[1]

    @property
    def c(self) -> float:
        return self[2]

    @property
    def d(self) -> float:
        return self[3]

    @property
    def normal(self) -> Vector3:
        return Vector3(self.a, self.b, self.c)

    @property
    def direction(self) -> Vector3:
        return self.normal.normalized
