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
from brayns.utils import ImageFormat, Resolution, serialize_view

from ..camera import Camera
from ..renderer import Renderer
from .key_frame import KeyFrame


@dataclass
class FrameExporter:
    """Frame exporter to take multiple snapshots in an optimized way.

    Camera position can be different for each frame using KeyFrame objects.

    Does not modify the current settings of the instance (resolution, camera,
    etc...).

    For parameters with None value, the current ones of the instance are used.

    Exported frames are named using 5 digits filenames (00000.png, 00001.png,
    00003.png, etc...).

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
        params = _serialize_exporter(self, folder)
        instance.request('export-frames', params)


def _serialize_exporter(exporter: FrameExporter, folder: str) -> dict[str, Any]:
    message: dict[str, Any] = {
        'path': folder,
        'key_frames': [
            _serialize_key_frame(frame)
            for frame in exporter.frames
        ],
        'image_settings': _serialize_image_settings(exporter),
    }
    if exporter.camera is not None:
        message['camera'] = exporter.camera.get_properties_with_name()
    if exporter.renderer is not None:
        message['renderer'] = exporter.renderer.get_properties_with_name()
    return message


def _serialize_key_frame(frame: KeyFrame) -> dict[str, Any]:
    message: dict[str, Any] = {
        'frame_index': frame.index,
    }
    if frame.view is not None:
        message['camera_view'] = serialize_view(frame.view)
    return message


def _serialize_image_settings(exporter: FrameExporter) -> dict[str, Any]:
    message: dict[str, Any] = {
        'format': exporter.format.value,
    }
    if exporter.format is ImageFormat.JPEG:
        message['quality'] = exporter.jpeg_quality
    if exporter.resolution is not None:
        message['size'] = list(exporter.resolution)
    return message
