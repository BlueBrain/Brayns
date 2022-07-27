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

import math

from brayns.core.camera.camera_view import CameraView
from brayns.core.common.bounds import Bounds
from brayns.core.common.vector3 import Vector3


class Fovy:

    def __init__(self, angle: float = math.radians(45), degrees: bool = False) -> None:
        self._angle = math.radians(angle) if degrees else angle

    def __eq__(self, other: object) -> bool:
        if not isinstance(other, Fovy):
            return False
        return self._angle == other._angle

    @property
    def radians(self) -> float:
        return self._angle

    @property
    def degrees(self) -> float:
        return math.degrees(self._angle)

    def get_full_screen_view(self, target: Bounds) -> CameraView:
        center = target.center
        distance = target.height / 2 / math.tan(self.radians / 2)
        distance += target.depth / 2
        position = center + distance * Vector3.forward
        return CameraView(position, center)
