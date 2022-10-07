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

from brayns.network import Instance
from brayns.utils import ImageFormat, Resolution, parse_image_format, serialize_view

from ..camera import Camera
from ..renderer import Renderer


@dataclass
class Snapshot:
    """Snapshot to render a given frame.

    For None parameters, the current values of the instance are used.

    Note: snapshots create a temporary context in the instance to avoid
    overriding current instance settings. Therefore, for multiple snapshots, it
    is more efficient to use a ``FrameExporter``.

    :param resolution: Image resolution, defaults to None.
    :type resolution: Resolution | None, optional
    :param frame: Simulation index, defaults to None.
    :type frame: int | None, optional
    :param camera: Camera used to render, defaults to None.
    :type camera: Camera | None, optional
    :param renderer: Renderer used to render, defaults to None.
    :type renderer: Renderer | None, optional
    :param jpeg_quality: JPEG quality if format is JPEG, defaults to 100.
    :type jpeg_quality: int, optional
    """

    resolution: Resolution | None = None
    frame: int | None = None
    camera: Camera | None = None
    renderer: Renderer | None = None
    jpeg_quality: int = 100

    def save(self, instance: Instance, path: str) -> None:
        """Download and save the snapshot locally under given file.

        Path is on the local machine (running current script).

        :param instance: Instance.
        :type instance: Instance
        :param path: Output file.
        :type path: str
        """
        format = parse_image_format(path)
        data = self.download(instance, format)
        with open(path, 'wb') as file:
            file.write(data)

    def save_remotely(self, instance: Instance, path: str) -> None:
        """Save the snapshot remotely under given file.

        Path is on the remote machine (running instance backend).

        :param instance: Instance.
        :type instance: Instance
        :param path: Output file.
        :type path: str
        """
        format = parse_image_format(path)
        params = _serialize_snapshot(self, format, path)
        _request(instance, params)

    def download(self, instance: Instance, format: ImageFormat = ImageFormat.PNG) -> bytes:
        """Download the rendered image as bytes at given format.

        :param instance: Instance.
        :type instance: Instance
        :param path: Output file.
        :type path: str
        """
        params = _serialize_snapshot(self, format)
        result = _request(instance, params)
        return base64.b64decode(result['data'])


def _serialize_snapshot(snapshot: Snapshot, format: ImageFormat, path: str | None = None) -> dict[str, Any]:
    message: dict[str, Any] = {
        'image_settings': _serialize_image_settings(snapshot, format),
    }
    if path is not None:
        message['file_path'] = path
    if snapshot.frame is not None:
        message['simulation_frame'] = snapshot.frame
    if snapshot.camera is not None:
        camera = snapshot.camera
        message['camera_view'] = serialize_view(camera.view)
        message['camera'] = camera.projection.get_properties_with_name()
    if snapshot.renderer is not None:
        message['renderer'] = snapshot.renderer.get_properties_with_name()
    return message


def _serialize_image_settings(snapshot: Snapshot, format: ImageFormat) -> dict[str, Any]:
    message: dict[str, Any] = {
        'format': format.value,
    }
    if format is ImageFormat.JPEG:
        message['quality'] = snapshot.jpeg_quality
    if snapshot.resolution is not None:
        message['size'] = list(snapshot.resolution)
    return message


def _request(instance: Instance, params: dict[str, Any]) -> dict[str, Any]:
    return instance.request('snapshot', params)