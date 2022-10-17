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
from typing import Any

from brayns.utils import Bounds
from brayns.utils import PlaneEquation
from brayns.utils.bounds import serialize_bounds

from .geometry import Geometry


@dataclass
class BoundedPlane(Geometry):
    """Axis-aligned bounded plane.

    :param equation: Plane equation coefficients.
    :type equation: PlaneEquation
    :param bounds: Axis aligned bounds to limit the plane.
    :type bounds: Bounds
    """

    equation: PlaneEquation
    bounds: Bounds

    def __new__(cls, equation: PlaneEquation, bounds: Bounds) -> BoundedPlane:
        if min(bounds.size) <= 0:
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
            'coefficients': list(self.equation),
            'bounds': serialize_bounds(self.bounds),
        }
