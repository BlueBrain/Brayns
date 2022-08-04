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

from brayns.core.camera.camera import Camera
from brayns.core.camera.camera_handler import CameraHandler
from brayns.core.camera.camera_registry import camera_registry
from brayns.core.view.fovy import Fovy


@dataclass
class PerspectiveCamera(Camera):

    fovy: Fovy = Fovy()
    aperture_radius: float = 0.0
    focus_distance: float = 1.0

    @classmethod
    @property
    def name(cls) -> str:
        return 'perspective'


class _Handler(CameraHandler):

    def deserialize(self, message: dict[str, Any]) -> PerspectiveCamera:
        return PerspectiveCamera(
            fovy=Fovy(message['fovy'], degrees=True),
            aperture_radius=message['aperture_radius'],
            focus_distance=message['focus_distance'],
        )

    def serialize(self, camera: PerspectiveCamera) -> dict[str, Any]:
        return {
            'fovy': camera.fovy.degrees,
            'aperture_radius': camera.aperture_radius,
            'focus_distance': camera.focus_distance,
        }


camera_registry.register(PerspectiveCamera, _Handler())
