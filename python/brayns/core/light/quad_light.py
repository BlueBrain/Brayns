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

from brayns.utils import Axis, Vector3

from .light import Light


@dataclass
class QuadLight(Light):
    """Rectangular light.

    Emission direction is the positive side (see emission_direction).

    :param bottom_left: Bottom left corner, defaults to origin.
    :type bottom_left: Vector3
    :param edge1: First edge, defaults to +X.
    :type edge1: Vector3, optional
    :param edge2: Second edge, defaults to +Y.
    :type edge2: Vector3, optional
    """

    bottom_left: Vector3 = Vector3.zero
    edge1: Vector3 = Axis.right
    edge2: Vector3 = Axis.up

    @classmethod
    @property
    def name(cls) -> str:
        """Return light name.

        :return: Light name.
        :rtype: str
        """
        return 'quad'

    @property
    def emission_direction(self) -> Vector3:
        """Get the emission direction of the light.

        Equal to edge1 x edge2 normalized.

        :return: Emission direction.
        :rtype: Vector3
        """
        return self.edge1.cross(self.edge2).normalized

    def get_additional_properties(self) -> dict[str, Any]:
        """Low level API to serialize to JSON."""
        return {
            'position': list(self.bottom_left),
            'edge1': list(self.edge1),
            'edge2': list(self.edge2),
        }
