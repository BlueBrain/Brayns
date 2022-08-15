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
    target: Vector3 = Vector3.forward
    up: Vector3 = Vector3.up

    @staticmethod
    def deserialize(message: dict[str, Any]) -> View:
        """Low level API to deserialize from JSON."""
        """Low level API to deserialize from JSON."""
        return View(
            position=Vector3(*message['position']),
            target=Vector3(*message['target']),
            up=Vector3(*message['up'])
        )

    @property
    def axis(self) -> Vector3:
        """Get view axis (target - position).

        :return: View axis.
        :rtype: Vector3
        """
        return self.target - self.position

    @property
    def direction(self) -> Vector3:
        """Get normalized direction (self.axis.normalized).

        :return: View direction normalized.
        :rtype: Vector3
        """
        return self.axis.normalized

    def serialize(self) -> dict[str, Any]:
        """Low level API to serialize to JSON."""
        return {
            'position': list(self.position),
            'target': list(self.target),
            'up': list(self.up)
        }
