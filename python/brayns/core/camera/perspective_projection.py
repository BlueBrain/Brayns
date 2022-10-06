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

from brayns.utils import Fovy

from .projection import Projection


@dataclass
class PerspectiveProjection(Projection):
    """Perspective camera projection.

    Perspective projection use a field of view angle to compute the size of the
    objects depending on their distance from the camera.

    The field of view (fovy) can be used to compute full screen view of a given
    target object.

    :param fovy: Field of view angle (45 degrees by default).
    :type fovy: Fovy
    :param aperture_radius: Optional aperture radius.
    :type aperture_radius: float
    :param focus_distance: Optional focus distance.
    :type focus_distance: float
    """

    fovy: Fovy = Fovy(45, degrees=True)
    aperture_radius: float = 0.0
    focus_distance: float = 1.0

    @classmethod
    @property
    def name(cls) -> str:
        """Camera name.

        :return: Camera name.
        :rtype: str
        """
        return 'perspective'

    def look_at(self, height: float) -> float:
        """Compute camera distance using field of view.

        :param height: Target height.
        :type target: float
        :return: Distance to see target entirely.
        :rtype: float
        """
        return self.fovy.get_distance(height)

    def get_properties(self) -> dict[str, Any]:
        """Low level API to serialize to JSON."""
        return {
            'fovy': self.fovy.degrees,
            'aperture_radius': self.aperture_radius,
            'focus_distance': self.focus_distance,
        }

    def update_properties(self, message: dict[str, Any]) -> None:
        """Low level API to deserialize from JSON."""
        self.fovy = Fovy(message['fovy'], degrees=True)
        self.aperture_radius = message['aperture_radius']
        self.focus_distance = message['focus_distance']
