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
from typing import Optional

from brayns.core.camera.camera import Camera
from brayns.core.common.resolution import Resolution
from brayns.core.renderer.renderer import Renderer
from brayns.core.snapshot.image_format import ImageFormat
from brayns.core.snapshot.key_frame import KeyFrame
from brayns.instance.instance import Instance


@dataclass
class FrameExporter:

    frames: list[KeyFrame]
    format: ImageFormat = ImageFormat.PNG
    jpeg_quality: int = 100
    resolution: Optional[Resolution] = None
    camera: Optional[Camera] = None
    renderer: Optional[Renderer] = None
    sequential_naming: bool = True

    def export_frames(self, instance: Instance, destination_folder: str) -> None:
        params = self._get_params(destination_folder)
        self._request(instance, params)

    def _get_params(self, path: str) -> dict:
        message = {
            'path': path,
            'key_frames': [
                frame.serialize()
                for frame in self.frames
            ],
            'sequential_naming': self.sequential_naming
        }
        image_settings = {
            'format': self.format.value
        }
        if self.format is ImageFormat.JPEG:
            image_settings['quality'] = self.jpeg_quality
        if self.resolution is not None:
            image_settings['size'] = list(self.resolution)
        message['image_settings'] = image_settings
        if self.camera is not None:
            message['camera'] = self.camera.serialize()
        if self.renderer is not None:
            message['renderer'] = self.renderer.serialize()
        return message

    def _request(self, instance: Instance, params: dict) -> None:
        instance.request('export-frames', params)
