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

import base64
from dataclasses import dataclass
from typing import Any

from brayns.core.camera.camera import Camera
from brayns.core.image.image_format import ImageFormat
from brayns.core.image.parse_image_format import parse_image_format
from brayns.core.image.resolution import Resolution
from brayns.core.renderer.renderer import Renderer
from brayns.core.view.view import View
from brayns.instance.instance import Instance


@dataclass
class Snapshot:

    resolution: Resolution | None = None
    frame: int | None = None
    view: View | None = None
    camera: Camera | None = None
    renderer: Renderer | None = None
    jpeg_quality: int = 100

    def save(self, instance: Instance, path: str) -> None:
        format = parse_image_format(path)
        data = self.download(instance, format)
        with open(path, 'wb') as file:
            file.write(data)

    def save_remotely(self, instance: Instance, path: str) -> None:
        params = self.serialize_with_path(path)
        self._request(instance, params)

    def download(self, instance: Instance, format: ImageFormat = ImageFormat.PNG) -> bytes:
        params = self.serialize_with_format(format)
        result = self._request(instance, params)
        return base64.b64decode(result['data'])

    def serialize_with_format(self, format: ImageFormat) -> dict[str, Any]:
        return self._serialize(format=format)

    def serialize_with_path(self, path: str) -> dict[str, Any]:
        return self._serialize(path=path)

    def _serialize(self, format: ImageFormat | None = None, path: str | None = None) -> dict[str, Any]:
        message = {}
        if path is not None:
            message['file_path'] = path
        image_settings = {}
        if format is not None:
            image_settings['format'] = format.value
        if format is ImageFormat.JPEG:
            image_settings['quality'] = self.jpeg_quality
        if self.resolution is not None:
            image_settings['size'] = list(self.resolution)
        if image_settings:
            message['image_settings'] = image_settings
        if self.frame is not None:
            message['simulation_frame'] = self.frame
        if self.view is not None:
            message['camera_view'] = self.view.serialize()
        if self.camera is not None:
            message['camera'] = self.camera.serialize_with_name()
        if self.renderer is not None:
            message['renderer'] = self.renderer.serialize_with_name()
        return message

    def _request(self, instance: Instance, params: dict[str, Any]) -> dict[str, Any]:
        return instance.request('snapshot', params)
