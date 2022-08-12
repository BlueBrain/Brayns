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
from typing import Any

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

    @staticmethod
    def deserialize(message: dict[str, Any]) -> Transform:
        """Low level API to deserialize from JSON."""
        return Transform(
            translation=Vector3(*message['translation']),
            rotation=Rotation.deserialize(message['rotation']),
            scale=Vector3(*message['scale']),
        )

    @classmethod
    @property
    def identity(cls) -> Transform:
        """Create an identity transform (doesn't do anything).

        :return: Identity transform.
        :rtype: Transform
        """
        return Transform()

    def serialize(self) -> dict[str, Any]:
        """Low level API to serialize to JSON."""
        return {
            'translation': list(self.translation),
            'rotation': self.rotation.serialize(),
            'scale': list(self.scale),
        }
