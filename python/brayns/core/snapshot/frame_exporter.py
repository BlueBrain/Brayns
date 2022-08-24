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

from dataclasses import dataclass
from typing import Any

from brayns.network import Instance
from brayns.utils import ImageFormat, Resolution

from ..camera import Camera
from ..renderer import Renderer
from .key_frame import KeyFrame


@dataclass
class FrameExporter:
    """Frame exporter to take multiple snapshots in an optimized way.

    Camera position can be different for each frame using KeyFrame objects.

    For parameters with None value, the current ones of the instance are used.

    Exported frames are names using 5 digits filenames.

    For example 00000.png, 00001.png, 00003.png.

    :param frames: Like of key frames to export.
    :type frames: list[KeyFrame]
    :param format: Format of the exported frames, defaults to PNG.
    :type format: ImageFormat, optional
    :param resolution: Resolution of the exported frames, defaults to None.
    :type resolution: Resolution | None, optional
    :param camera: Camera to use for render, defaults to None.
    :type camera: Camera | None, optional
    :param renderer: Renderer to use for render, defaults to None.
    :type renderer: Renderer | None, optional
    :param jpeg_quality: JPEG quality if format is JPEG, defaults to 100.
    :type jpeg_quality: int, optional
    """

    frames: list[KeyFrame]
    format: ImageFormat = ImageFormat.PNG
    resolution: Resolution | None = None
    camera: Camera | None = None
    renderer: Renderer | None = None
    jpeg_quality: int = 100

    def export_frames(self, instance: Instance, folder: str) -> None:
        """Export frames to given folder.

        :param instance: Instance.
        :type instance: Instance
        :param folder: Output folder.
        :type folder: str
        """
        params = self.serialize(folder)
        instance.request('export-frames', params)

    def serialize(self, folder: str) -> dict[str, Any]:
        """Low level API to serialize to JSON."""
        message = {
            'path': folder,
            'key_frames': [
                frame.serialize()
                for frame in self.frames
            ]
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
            message['camera'] = self.camera.serialize_with_name()
        if self.renderer is not None:
            message['renderer'] = self.renderer.serialize_with_name()
        return message
