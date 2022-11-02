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
from .rotation import Rotation


@dataclass
class Transform:
    """3D transformation.

    Used to move models in 3D space.

    It is equivalent to a TRS matrix multiplication.

    It means we first scale the point at the origin, then rotate around the same
    origin and finally we translate it to its position.

    :param translation: Translation, defaults to zero.
    :type translation: Vector3, optional
    :param rotation: Rotation, defaults to identity.
    :type rotation: Rotation, optional
    :param scale: Scaling, defaults to one.
    :type scale: Vector3, optional
    """

    translation: Vector3 = Vector3.zero
    rotation: Rotation = Rotation.identity
    scale: Vector3 = Vector3.one

    @classmethod
    @property
    def identity(cls) -> Transform:
        """Create an identity transform (doesn't do anything).

        :return: Identity transform.
        :rtype: Transform
        """
        return Transform()

    def apply(self, value: Vector3) -> Vector3:
        """Apply the transform to the given value.

        :param value: 3D position to transform.
        :type value: Vector3
        :return: Transformed value.
        :rtype: Vector3
        """
        value = self.scale * value
        value = self.rotation.apply(value)
        return value + self.translation

    def combine(self, other: Transform) -> Transform:
        """Create another transform combining self and other.

        :param other: Transform to apply after self.
        :type other: Transform
        :return: Equivalent transform of self then other.
        :rtype: Transform
        """
        return Transform(
            translation=self.translation + other.translation,
            rotation=self.rotation.combine(other.rotation),
            scale=self.scale * other.scale,
        )

    def rotate(self, rotation: Rotation, center: Vector3 = Vector3.zero) -> Transform:
        """Create another transform rotated around a given center.

        :param rotation: Rotation to apply.
        :type rotation: Rotation
        :param center: Rotation center, defaults to zero.
        :type center: Vector3, optional
        :return: Rotated transform.
        :rtype: Transform
        """
        return Transform(
            translation=self.translation + center - rotation.apply(center),
            rotation=self.rotation.combine(rotation),
            scale=self.scale,
        )
