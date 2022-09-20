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

from brayns.utils import Vector3

from .geometry import Geometry


@dataclass
class Capsule(Geometry):
    """Capsule (cylinder with slope and half spheres at extremities).

    :param start_point: Cylinder base XYZ.
    :type start_point: Vector3
    :param start_radius: Cylinder and sphere base radius.
    :type start_radius: float
    :param end_point: Cylinder end XYZ.
    :type end_point: Vector3
    :param end_radius: Cylinder and sphere end radius.
    :type end_radius: float
    """

    start_point: Vector3
    start_radius: float
    end_point: Vector3
    end_radius: float

    @classmethod
    @property
    def method(cls) -> str:
        """JSON-RPC method to add capsules.

        :return: JSON-RPC method.
        :rtype: str
        """
        return 'add-capsules'

    def get_additional_properties(self) -> dict[str, Any]:
        """Low level API to serialize to JSON."""
        return {
            'p0': list(self.start_point),
            'r0': self.start_radius,
            'p1': list(self.end_point),
            'r1': self.end_radius,
        }
