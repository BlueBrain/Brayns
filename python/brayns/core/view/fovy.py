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

import math

from brayns.core.bounds import Bounds
from brayns.core.vector import Vector3

from .view import View


class Fovy:
    """Field of view (angle) of a camera."""

    def __init__(self, angle: float, degrees: bool = False) -> None:
        """Construct with angle.

        :param angle: Angle value.
        :type angle: float
        :param degrees: True if angle is in degrees, defaults to False
        :type degrees: bool, optional
        """
        self._angle = math.radians(angle) if degrees else angle

    def __eq__(self, other: object) -> bool:
        """Comparison operator.

        :param other: Object to compare.
        :type other: object
        :return: True if other is Fovy and has same value.
        :rtype: bool
        """
        if not isinstance(other, Fovy):
            return False
        return self._angle == other._angle

    @property
    def radians(self) -> float:
        """Get angle in radians.

        :return: Angle in radians.
        :rtype: float
        """
        return self._angle

    @property
    def degrees(self) -> float:
        """Get angle in degrees.

        :return: Angle in degrees.
        :rtype: float
        """
        return math.degrees(self._angle)

    def get_full_screen_view(self, target: Bounds) -> View:
        """Compute the view necessary to see the target in full screen.

        :param target: Bounds of the target to look.
        :type target: Bounds
        :return: Full screen view.
        :rtype: View
        """
        center = target.center
        distance = target.height / 2 / math.tan(self.radians / 2)
        distance += target.depth / 2
        position = center + distance * Vector3.forward
        return View(position, center)
