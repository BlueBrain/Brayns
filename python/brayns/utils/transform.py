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

from .quaternion import Quaternion
from .rotation import Rotation
from .vector import Vector3


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

    @staticmethod
    def rotate(rotation: Rotation, center: Vector3 = Vector3.zero) -> Transform:
        """Create a transform to rotate around a given center.

        Rotation around a center is equivalent to a rotation around the origin
        and a translation of center - rotation.apply(center).

        :param rotation: Rotation to apply.
        :type rotation: Rotation
        :param center: Rotation center, defaults to zero.
        :type center: Vector3, optional
        :return: Transform.
        :rtype: Transform
        """
        return Transform(
            translation=center - rotation.apply(center),
            rotation=rotation,
        )

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


def deserialize_transform(obj: dict[str, Any]) -> Transform:
    return Transform(
        translation=Vector3(*obj["translation"]),
        rotation=Rotation.from_quaternion(Quaternion(*obj["rotation"])),
        scale=Vector3(*obj["scale"]),
    )


def serialize_transform(transform: Transform) -> dict[str, Any]:
    return {
        "translation": list(transform.translation),
        "rotation": list(transform.rotation.quaternion),
        "scale": list(transform.scale),
    }
