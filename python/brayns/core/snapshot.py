# Copyright (c) 2015-2023 EPFL/Blue Brain Project
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

from brayns.network import Future, Instance, JsonRpcFuture, JsonRpcReply
from brayns.utils import ImageFormat, Resolution, parse_image_format

from .camera import Camera
from .renderer import Renderer
from .view import serialize_view


@dataclass
class Snapshot:
    """Helper class to take a snapshot of an instance with custom settings.

    Snapshots create a temporary context in the instance to avoid changing
    current instance settings.

    For None parameters, the current values of the instance are used.

    :param resolution: Image resolution, defaults to None.
    :type resolution: Resolution | None, optional
    :param camera: Camera used to render, defaults to None.
    :type camera: Camera | None, optional
    :param renderer: Renderer used to render, defaults to None.
    :type renderer: Renderer | None, optional
    :param frame: Simulation index, defaults to None.
    :type frame: int | None, optional
    :param jpeg_quality: JPEG quality if format is JPEG, defaults to 100%.
    :type jpeg_quality: int, optional
    """

    resolution: Resolution | None = None
    camera: Camera | None = None
    renderer: Renderer | None = None
    frame: int | None = None
    jpeg_quality: int = 100

    def save(self, instance: Instance, path: str) -> None:
        """Download and save the snapshot locally under given file.

        Path is on the local machine (running current script).

        :param instance: Instance.
        :type instance: Instance
        :param path: Output file.
        :type path: str
        """
        task = self.save_task(instance, path)
        task.wait_for_result()

    def save_task(self, instance: Instance, path: str) -> Future[None]:
        """Asynchronous version of ``save``.

        :param instance: Instance.
        :type instance: Instance
        :param path: Output file.
        :type path: str
        :return: Future to monitor the task.
        :rtype: Future[None]
        """
        format = parse_image_format(path)
        task = self._download(instance, format)
        return Future(task, lambda reply: _save_color_buffer(reply, path))

    def save_remotely(self, instance: Instance, path: str) -> None:
        """Save the snapshot remotely under given file.

        Path is on the remote machine (running instance backend).

        :param instance: Instance.
        :type instance: Instance
        :param path: Output file.
        :type path: str
        """
        task = self.save_remotely_task(instance, path)
        task.wait_for_result()

    def save_remotely_task(self, instance: Instance, path: str) -> Future[None]:
        """Asynchronous version of ``save_remotely``.

        :param instance: Instance.
        :type instance: Instance
        :param path: Output file.
        :type path: str
        :return: Future to monitor the task.
        :rtype: Future[None]
        """
        format = parse_image_format(path)
        params = _serialize_snapshot(self, format, path)
        task = _task(instance, params)
        return Future(task, lambda _: None)

    def download(
        self, instance: Instance, format: ImageFormat = ImageFormat.PNG
    ) -> bytes:
        """Download the rendered image as bytes at given format.

        :param instance: Instance.
        :type instance: Instance
        :param path: Output file.
        :type path: str
        :return: Image data.
        :rtype: bytes
        """
        task = self.download_task(instance, format)
        return task.wait_for_result()

    def download_task(
        self, instance: Instance, format: ImageFormat = ImageFormat.PNG
    ) -> Future[bytes]:
        """Asynchronous version of ``download``.

        :param instance: Instance.
        :type instance: Instance
        :param path: Output file.
        :type path: str
        :return: Future to monitor the task.
        :rtype: Future[bytes]
        """
        task = self._download(instance, format)
        return Future(task, _get_color_buffer)

    def _download(self, instance: Instance, format: ImageFormat) -> JsonRpcFuture:
        params = _serialize_snapshot(self, format)
        return _task(instance, params)


def _serialize_snapshot(
    snapshot: Snapshot, format: ImageFormat, path: str | None = None
) -> dict[str, Any]:
    message: dict[str, Any] = {
        "image_settings": _serialize_image_settings(snapshot, format),
    }
    if path is not None:
        message["file_path"] = path
    if snapshot.camera is not None:
        camera = snapshot.camera
        message["camera_view"] = serialize_view(camera.view)
        message["camera"] = camera.projection.get_properties_with_name()
    if snapshot.renderer is not None:
        message["renderer"] = snapshot.renderer.get_properties_with_name()
    if snapshot.frame is not None:
        message["simulation_frame"] = snapshot.frame
    return message


def _serialize_image_settings(
    snapshot: Snapshot, format: ImageFormat
) -> dict[str, Any]:
    message: dict[str, Any] = {
        "format": format.value,
    }
    if format is ImageFormat.JPEG:
        message["quality"] = snapshot.jpeg_quality
    if snapshot.resolution is not None:
        message["size"] = list(snapshot.resolution)
    return message


def _task(instance: Instance, params: dict[str, Any]) -> JsonRpcFuture:
    return instance.task("snapshot", params)


def _get_color_buffer(reply: JsonRpcReply) -> bytes:
    result, data = reply.result, reply.binary
    buffer = result["color_buffer"]
    offset = buffer["offset"]
    size = buffer["size"]
    return data[offset:size]


def _save_color_buffer(reply: JsonRpcReply, path: str) -> None:
    data = _get_color_buffer(reply)
    with open(path, "wb") as file:
        file.write(data)
