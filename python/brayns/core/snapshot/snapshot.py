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

import base64
from dataclasses import dataclass
from typing import Optional

from brayns.core.camera.camera import Camera
from brayns.core.camera.camera_view import CameraView
from brayns.core.common.resolution import Resolution
from brayns.core.renderer.renderer import Renderer
from brayns.core.snapshot.image_format import ImageFormat
from brayns.instance.instance import Instance


@dataclass
class Snapshot:

    jpeg_quality: int = 100
    resolution: Optional[Resolution] = None
    frame: Optional[int] = None
    view: Optional[CameraView] = None
    camera: Optional[Camera] = None
    renderer: Optional[Renderer] = None

    def save(self, instance: Instance, path: str) -> None:
        format = ImageFormat.from_path(path)
        data = self.download(instance, format)
        with open(path, 'wb') as file:
            file.write(data)

    def save_remotely(self, instance: Instance, path: str) -> None:
        format = ImageFormat.from_path(path)
        params = self._get_params(format, path)
        self._request(instance, params)

    def download(self, instance: Instance, format: ImageFormat = ImageFormat.PNG) -> bytes:
        params = self._get_params(format)
        result = self._request(instance, params)
        return base64.b64decode(result['data'])

    def _get_params(self, format: ImageFormat, path: Optional[str] = None) -> dict:
        message = {}
        if path is not None:
            message['path'] = path
        image_settings = {
            'format': format.value
        }
        if format is ImageFormat.JPEG:
            image_settings['quality'] = self.jpeg_quality
        if self.resolution is not None:
            image_settings['size'] = list(self.resolution)
        message['image_settings'] = image_settings
        if self.frame is not None:
            message['animation_settings'] = {'frame': self.frame}
        if self.view is not None:
            message['camera_view'] = self.view.serialize()
        if self.camera is not None:
            message['camera'] = self.camera.serialize()
        if self.renderer is not None:
            message['renderer'] = self.renderer.serialize()
        return message

    def _request(self, instance: Instance, params: dict) -> dict:
        return instance.request('snapshot', params)
