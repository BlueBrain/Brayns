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

from brayns.utils import Bounds, Vector3, View

from .projection import Projection


@dataclass
class OrthographicProjection(Projection):
    """Orthographic camera projection.

    Orthographic camera makes all objects having the same size regardless their
    distance from the camera.

    The viewport width is computed using the aspect ratio of the current
    resolution of the instance (framebuffer size).

    :param height: Viewport height in world coordinates.
    :type height: float
    """

    height: float = 0.0

    @classmethod
    @property
    def name(cls) -> str:
        """Projection name.

        :return: Projection name.
        :rtype: str
        """
        return 'orthographic'

    def look_at(self, target: Bounds, aspect_ratio: float) -> View:
        """Adjust camera viewport to look at target.

        Distance from the object doesn't matter as long as no other objects are
        between the camera and the target.

        By default, the margin between target surface and the camera is half of
        the target depth.

        :param target: Camera target.
        :type target: Bounds
        :param aspect_ratio: Viewport aspect ratio.
        :type aspect_ratio: float
        :return: Front view to see the target entirely.
        :rtype: View
        """
        center = target.center
        width, height, depth = target.size
        self.height = max(height, width / aspect_ratio)
        position = center + depth * Vector3.forward
        return View(position, center)

    def get_properties(self) -> dict[str, Any]:
        """Low level API to serialize to JSON."""
        return {
            'height': self.height,
        }

    def update_properties(self, message: dict[str, Any]) -> None:
        """Low level API to deserialize from JSON."""
        self.height = message['height']
