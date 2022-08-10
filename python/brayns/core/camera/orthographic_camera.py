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
from typing import Any, TypeVar

from brayns.core.bounds import Bounds
from brayns.core.vector import Vector3
from brayns.core.view import View

from camera import Camera

T = TypeVar('T', bound='OrthographicCamera')


@dataclass
class OrthographicCamera(Camera):
    """Orthographic camera.

    Orthographic camera makes all objects having the same size regardless their
    distance from the camera.

    The viewport width is computed using the aspect ratio of the current
    resolution of the renderer (framebuffer size).

    :param height: Viewport height in world coordinates.
    :type height: float
    """

    height: float = 0.0

    @staticmethod
    def get_front_view(target: Bounds) -> View:
        """Helper method to get the front view of a target object.

        Distance from the object doesn't matter as long as no other objects are
        between the camera and the target.

        By default the camera-target distance is half of the target depth.

        :param target: _description_
        :type target: Bounds
        :return: _description_
        :rtype: View
        """
        center = target.center
        distance = target.depth
        position = center + distance * Vector3.forward
        return View(position, center)

    @classmethod
    @property
    def name(cls) -> str:
        """Camera name.

        :return: Camera name.
        :rtype: str
        """
        return 'orthographic'

    @classmethod
    def deserialize(cls: type[T], message: dict[str, Any]) -> T:
        """Low level API to deserialize from JSON."""
        return cls(
            height=message['height']
        )

    def serialize(self) -> dict[str, Any]:
        """Low level API to serialize to JSON."""
        return {
            'height': self.height
        }
