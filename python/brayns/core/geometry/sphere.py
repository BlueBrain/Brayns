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
class Sphere(Geometry):
    """Sphere with radius and position.

    :param radius: Radius.
    :type radius: float
    :param center: Center XYZ, defaults to origin.
    :type center: Vector3
    """

    radius: float
    center: Vector3 = Vector3.zero

    @classmethod
    @property
    def method(cls) -> str:
        """JSON-RPC method to add spheres.

        :return: JSON-RPC method.
        :rtype: str
        """
        return 'add-spheres'

    def get_additional_properties(self) -> dict[str, Any]:
        """Low level API to serialize to JSON."""
        return {
            'center': list(self.center),
            'radius': self.radius,
        }
