# Copyright (c) 2015-2023 EPFL/Blue Brain Project
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
from typing import Any

from .transform import Rotation
from .vector import Vector3, componentwise_max, componentwise_min


@dataclass
class Bounds:
    """Axis aligned bounding box.

    :param min: Minimum XYZ.
    :type min: Vector3
    :param max: Maximum XYZ.
    :type max: Vector3
    """

    min: Vector3
    max: Vector3

    @staticmethod
    def of(values: list[Vector3]) -> Bounds:
        """Compute the bounds of given 3D points.

        :param values: List of points to include in bounds.
        :type values: list[Vector3]
        :return: Points boundary
        :rtype: Bounds
        """
        return Bounds(
            min=componentwise_min(values),
            max=componentwise_max(values),
        )

    @classmethod
    @property
    def empty(cls) -> Bounds:
        """Create empty bounds.

        :return: Empty bounds in [0, 0, 0].
        :rtype: Bounds
        """
        return Bounds(Vector3.zero, Vector3.zero)

    @classmethod
    @property
    def unit(cls) -> Bounds:
        """Create unit bounds (size = [1, 1, 1]).

        :return: Unit bounds centered in [0, 0, 0].
        :rtype: Bounds
        """
        return Bounds(-Vector3.one / 2, Vector3.one / 2)

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

    @property
    def corners(self) -> list[Vector3]:
        """List the 8 corners of the box.

        :return: List of corner points.
        :rtype: list[Vector3]
        """
        return [
            self.min,
            Vector3(self.min.x, self.min.y, self.max.z),
            Vector3(self.min.x, self.max.y, self.min.z),
            Vector3(self.min.x, self.max.y, self.max.z),
            Vector3(self.max.x, self.min.y, self.min.z),
            Vector3(self.max.x, self.min.y, self.max.z),
            Vector3(self.max.x, self.max.y, self.min.z),
            self.max,
        ]

    def translate(self, translation: Vector3) -> Bounds:
        """Translate bounds by given value.

        :param translation: Translation for min and max.
        :type translation: Vector3
        :return: Translated bounds.
        :rtype: Bounds
        """
        return Bounds(
            min=self.min + translation,
            max=self.max + translation,
        )

    def rotate(self, rotation: Rotation, center: Vector3 = Vector3.zero) -> Bounds:
        """Rotate bounds by given value around optional center.

        :param rotation: Rotation to apply.
        :type rotation: Rotation
        :param center: Rotation center, defaults to Vector3.zero.
        :type center: Vector3, optional
        :return: New axis aligned bounds after rotation.
        :rtype: Bounds
        """
        return Bounds.of([rotation.apply(corner, center) for corner in self.corners])

    def rescale(self, scale: Vector3) -> Bounds:
        """Multiply limits by given scale componentwise.

        :param scale: Scale XYZ.
        :type scale: Vector3
        :return: Rescaled bounds.
        :rtype: Bounds
        """
        return Bounds(
            min=scale * self.min,
            max=scale * self.max,
        )


def merge_bounds(values: list[Bounds]) -> Bounds:
    """Compute the union of all given bounds.

    Returns Bounds.empty if values are empty.

    Assume that all bounds are valid (ie min <= max for each component).

    :param values: Bounds to merge.
    :type values: list[Bounds]
    :return: Union of all bounds in values.
    :rtype: Bounds
    """
    return Bounds(
        min=componentwise_min([value.min for value in values]),
        max=componentwise_max([value.max for value in values]),
    )


def deserialize_bounds(obj: dict[str, Any]) -> Bounds:
    return Bounds(
        min=Vector3(*obj["min"]),
        max=Vector3(*obj["max"]),
    )


def serialize_bounds(bounds: Bounds) -> dict[str, Any]:
    return {
        "min": list(bounds.min),
        "max": list(bounds.max),
    }
