# Copyright (c) 2015-2022 EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
#
# Responsible Author: nadir.romanguerrero@epfl.ch
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

from dataclasses import dataclass
from multiprocessing.sharedctypes import Value
from typing import Any

from .geometry import Geometry
from .box import Box


@dataclass
class BoundedPlane(Geometry):
    """Axis-aligned bounded plane.

    Described by the equation ``ax + by + cz + d = 0``.

    Where [a, b, c] is the normal of the plane and d the orthogonal distance
    from the origin.

    :param a: X term of the plane equation.
    :type a: float
    :param b: Y term of the plane equation.
    :type b: float
    :param c: Y term of the plane equation.
    :type c: float
    :param d: Scalar term of the plane equation.
    :type d: float
    :param bounds: Axis aligned bounds to limit the plane.
    :type bounds: Box
    """

    a: float
    b: float
    c: float
    d: float
    bounds: Box

    def __new__(cls, a: float, b: float, c: float, d: float, bounds: Box) -> BoundedPlane:
        diff = bounds.max - bounds.min
        if diff.x <= 0 or diff.y <= 0 or diff.z <= 0:
            raise ValueError("All bound dimensions must be greater than 0")
        return object.__new__(cls)

    @classmethod
    @property
    def method(cls) -> str:
        """JSON-RPC method to add bounded planes.

        :return: JSON-RPC method.
        :rtype: str
        """
        return 'add-bounded-planes'

    def get_additional_properties(self) -> dict[str, Any]:
        """Low level API to serialize to JSON."""
        return {
            'coefficients': [self.a, self.b, self.c, self.d],
            'bounds': self.bounds.get_additional_properties(),
        }
