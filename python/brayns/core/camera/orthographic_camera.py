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

from brayns.core.camera.camera import Camera
from brayns.core.camera.camera_view import CameraView
from brayns.core.common.bounds import Bounds
from brayns.core.common.vector3 import Vector3


@dataclass
class OrthographicCamera(Camera):

    height: float = 0.0

    @staticmethod
    def from_target(target: Bounds) -> 'OrthographicCamera':
        return OrthographicCamera(target.height)

    @classmethod
    @property
    def name(cls) -> str:
        return 'orthographic'

    @classmethod
    def deserialize(cls, message: dict) -> 'OrthographicCamera':
        return OrthographicCamera(
            height=message['height'],
        )

    def serialize(self) -> dict:
        return {
            'height': self.height
        }

    def get_full_screen_view(self, target: Bounds) -> CameraView:
        center = target.center
        distance = target.depth
        position = center + distance * Vector3.forward
        return CameraView(position, center)
