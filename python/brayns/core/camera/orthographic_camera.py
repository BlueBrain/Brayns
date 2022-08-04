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


@dataclass
class OrthographicCamera(Camera):

    height: float = 0.0

    @classmethod
    @property
    def name(cls) -> str:
        return 'orthographic'


class _Handler(CameraHandler):

    def deserialize(self, message: dict[str, Any]) -> OrthographicCamera:
        return OrthographicCamera(
            height=message['height']
        )

    def serialize(self, camera: OrthographicCamera) -> dict[str, Any]:
        return {
            'height': camera.height
        }


camera_registry.register(OrthographicCamera, _Handler())
