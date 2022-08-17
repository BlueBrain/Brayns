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

from dataclasses import dataclass
from typing import Any

from .clipping_geometry import ClippingGeometry


@dataclass
class ClipPlane(ClippingGeometry):
    """Clip plane.

    Described by the equation `ax + by + cz + d = 0`.

    Where [a, b, c] is the normal of the plane and d the orthogonal distance
    from the origin.

    The normal vector indicates the upper side of the plane.

    Everything below the given plane is clipped:

    Multiple clip planes can be combined to slice a model in complex ways.

    :param a: X term of the plane equation.
    :type a: float
    :param b: Y term of the plane equation.
    :type b: float
    :param c: Y term of the plane equation.
    :type c: float
    :param d: Scalar term of the plane equation.
    :type d: float
    """

    a: float
    b: float
    c: float
    d: float = 0.0

    @classmethod
    @property
    def method(cls) -> str:
        """Get the JSON-RCP method to add the clipping geometry.

        :return: JSON-RPC method.
        :rtype: str
        """
        return 'add-clip-plane'

    def serialize(self) -> dict[str, Any]:
        """Low level API to serialize to JSON."""
        return {
            'coefficients': [self.a, self.b, self.c, self.d]
        }
