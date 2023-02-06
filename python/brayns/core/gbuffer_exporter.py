# Copyright (c) 2015-2023 EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.

# Responsible Authors:
#   Adrien Christian Fleury <adrien.fleury@epfl.ch>
#   Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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
from enum import Enum
from pathlib import Path
from typing import Any

from brayns.network import Future, Instance, JsonRpcFuture, JsonRpcReply
from brayns.utils import Resolution

from .camera import Camera
from .renderer import Renderer
from .view import serialize_view


class GBufferChannel(Enum):
    """All available channels to export."""

    COLOR = "color"
    DEPTH = "depth"
    ALBEDO = "albedo"
    NORMAL = "normal"


@dataclass
class GBufferExporter:
    """Helper class to export the G-Buffers of an instance as an EXR encoded
    image.

    A temporary context will be created in the instance to avoid changing
    its state.

    For None parameters, the current values of the instance are used.

    :param channels: List of g-buffer channels to export.
    :type channels: list[GBufferChannel]
    :param resolution: Image resolution, defaults to None.
    :type resolution: Resolution | None, optional
    :param camera: Camera used to render, defaults to None.
    :type camera: Camera | None, optional
    :param renderer: Renderer used to render, defaults to None.
    :type renderer: Renderer | None, optional
    :param frame: Simulation index, defaults to None.
    :type frame: int | None, optional
    """

    channels: list[GBufferChannel]
    resolution: Resolution | None = None
    camera: Camera | None = None
    renderer: Renderer | None = None
    frame: int | None = None

    def save(self, instance: Instance, path: str) -> None:
        """Download and save the g-buffers locally under given file.

        Path is on the local machine (running current script).

        The ".exr" extension will be appended automatically if not present.

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

        if Path(path).suffix != ".exr":
            path = path + ".exr"

        task = self._download(instance)
        return Future(task, lambda reply: _save_exr(reply, path))

    def save_remotely(self, instance: Instance, path: str) -> None:
        """Exports the g-buffers remotely under given file.

        Path is on the remote machine (running instance backend).

        The ".exr" extension will be appended automatically if not present.

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
        params = _serialize_export(self, path)
        task = _task(instance, params)
        return Future(task, lambda _: None)

    def download(self, instance: Instance) -> bytes:
        """Download the exported g-buffers as EXR encoded bytes.

        :param instance: Instance.
        :type instance: Instance
        :return: Image data.
        :rtype: bytes
        """
        task = self.download_task(instance)
        return task.wait_for_result()

    def download_task(self, instance: Instance) -> Future[bytes]:
        """Asynchronous version of ``download``.

        :param instance: Instance.
        :type instance: Instance
        :return: Future to monitor the task.
        :rtype: Future[bytes]
        """
        task = self._download(instance)
        return Future(task, lambda reply: reply.binary)

    def _download(self, instance: Instance) -> JsonRpcFuture:
        params = _serialize_export(self)
        return _task(instance, params)


def _serialize_export(
    export: GBufferExporter, path: str | None = None
) -> dict[str, Any]:
    message: dict[str, Any] = {
        "channels": [channel.value for channel in export.channels]
    }

    if path is not None:
        message["file_path"] = path
    if export.resolution is not None:
        message["resolution"] = list(export.resolution)
    if export.camera is not None:
        camera = export.camera
        message["camera_view"] = serialize_view(camera.view)
        message["camera"] = camera.projection.get_properties_with_name()
    if export.renderer is not None:
        message["renderer"] = export.renderer.get_properties_with_name()
    if export.frame is not None:
        message["simulation_frame"] = export.frame

    return message


def _task(instance: Instance, params: dict[str, Any]) -> JsonRpcFuture:
    return instance.task("export-gbuffers", params)


def _save_exr(reply: JsonRpcReply, path: str) -> None:
    data = reply.binary
    with open(path, "wb") as file:
        file.write(data)
