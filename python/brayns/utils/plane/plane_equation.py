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

from ..vector import Vector


class PlaneEquation(Vector[float]):
    """Plane equation such as ax + by + cz + d = 0.

    :param x: X component.
    :type x: float
    :param y: Y component.
    :type y: float
    :param z: Z component.
    :type z: float
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
