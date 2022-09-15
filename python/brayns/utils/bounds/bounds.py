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

from dataclasses import dataclass

from ..vector import Vector3


@dataclass
class Bounds:
    """3D boundary in world coordinates.

    :param min: Minimum XYZ.
    :type min: Vector3
    :param max: Maximum XYZ.
    :type max: Vector3
    """

    min: Vector3
    max: Vector3

    @classmethod
    @property
    def empty(cls) -> Bounds:
        """Create empty bounds.

        :return: Empty bounds in [0, 0, 0].
        :rtype: Bounds
        """
        return Bounds(Vector3.zero, Vector3.zero)

    @property
    def center(self) -> Vector3:
        """Compute the center point of the bounds.

        :return: 3D point of the center.
        :rtype: Vector3
        """
        return (self.min + self.max) / 2

    @property
    def size(self) -> Vector3:
        """Compute the size of the bounds.

        :return: Size XYZ (width, height, depth).
        :rtype: Vector3
        """
        return self.max - self.min

    @property
    def width(self) -> float:
        """Compute the width of the bounds.

        :return: size.x.
        :rtype: float
        """
        return self.size.x

    @property
    def height(self) -> float:
        """Compute the height of the bounds.

        :return: size.y.
        :rtype: float
        """
        return self.size.y

    @property
    def depth(self) -> float:
        """Compute the depth of the bounds.

        :return: size.z.
        :rtype: float
        """
        return self.size.z
