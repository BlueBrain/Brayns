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

from ..transform import Rotation
from ..vector import Axis, Vector3


@dataclass
class View:
    """Represent a viewpoint with position and target.

    :param position: Observation position.
    :type position: Vector3
    :param target: Target position.
    :type target: Vector3
    :param up: Up vector, defaults to +Y (global up).
    :type up: Vector3, optional
    """

    position: Vector3 = Vector3.zero
    target: Vector3 = Axis.front
    up: Vector3 = Axis.up

    @property
    def vector(self) -> Vector3:
        """Get view vector (target - position).

        :return: View vector.
        :rtype: Vector3
        """
        return (self.target - self.position)

    @property
    def direction(self) -> Vector3:
        """Get normalized view vector.

        :return: View vector normalized.
        :rtype: Vector3
        """
        return self.vector.normalized

    @property
    def distance(self) -> float:
        """Get the distance between the observator and the target.

        :return: View vector norm.
        :rtype: float
        """
        return self.vector.norm

    @distance.setter
    def distance(self, value: float) -> None:
        """Move the observer position to be at given distance from target.

        View direction remains unchanged.

        :param value: New observer distance.
        :type value: float
        """
        self.position = self.target - value * self.direction

    def translate(self, translation: Vector3) -> View:
        """Translate position and target in given direction.

        :param translation: Translation to apply on position and target.
        :type translation: Vector3
        :return: New translated view.
        :rtype: View
        """
        return View(
            position=self.position + translation,
            target=self.target + translation,
            up=self.up,
        )

    def rotate_around_target(self, rotation: Rotation) -> View:
        """Rotate observation position around target.

        :param rotation: Rotation to apply on observer.
        :type rotation: Rotation
        :return: New rotated view.
        :rtype: View
        """
        return View(
            position=rotation.apply(self.position, center=self.target),
            target=self.target,
            up=rotation.apply(self.up),
        )
