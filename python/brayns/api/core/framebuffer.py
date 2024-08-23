# Copyright (c) 2015-2024 EPFL/Blue Brain Project
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

from dataclasses import dataclass, field, replace
from enum import Enum
from typing import Any, NewType

from brayns.network.connection import Connection
from brayns.utils.parsing import get, get_tuple, try_get

from .image_operation import ImageOperationId
from .objects import create_specific_object, get_specific_object, update_specific_object

FramebufferId = NewType("FramebufferId", int)


class FramebufferFormat(Enum):
    RGBA8 = "Rgba8"
    SRGBA8 = "Srgba8"
    RGBA32F = "Rgba32F"


class FramebufferChannel(Enum):
    COLOR = "Color"
    DEPTH = "Depth"
    NORMAL = "Normal"
    ALBEDO = "Albedo"
    PRIMITIVE_ID = "PrimitiveId"
    MODEL_ID = "ModelId"
    INSTANCE_ID = "InstanceId"


@dataclass
class Accumulation:
    variance: bool = False


@dataclass
class FramebufferSettings:
    resolution: tuple[int, int] = (1920, 1080)
    format: FramebufferFormat = FramebufferFormat.SRGBA8
    channels: set[FramebufferChannel] = field(default_factory=lambda: {FramebufferChannel.COLOR})
    accumulation: Accumulation | None = None
    image_operations: set[ImageOperationId] = field(default_factory=set)


def serialize_framebuffer_settings(settings: FramebufferSettings) -> dict[str, Any]:
    accumulation = settings.accumulation
    accumulation = None if accumulation is None else {"variance": accumulation.variance}

    return {
        "resolution": list(settings.resolution),
        "format": settings.format.value,
        "channels": [channel.value for channel in settings.channels],
        "accumulation": accumulation,
        "imageOperations": list(settings.image_operations),
    }


def deserialize_framebuffer_settings(message: dict[str, Any]) -> FramebufferSettings:
    accumulation = try_get(message, "accumulation", dict[str, Any])
    accumulation = None if accumulation is None else Accumulation(get(accumulation, "variance", bool))

    return FramebufferSettings(
        resolution=tuple(get_tuple(message, "resolution", int, 2)),
        format=FramebufferFormat(get(message, "format", str)),
        channels={FramebufferChannel(value) for value in get(message, "channels", list[str])},
        accumulation=accumulation,
        image_operations=set(get(message, "imageOperations", list[int])),
    )


@dataclass
class FramebufferInfo:
    settings: FramebufferSettings
    variance: float | None = None


def deserialize_framebuffer_info(message: dict[str, Any]) -> FramebufferInfo:
    return FramebufferInfo(
        settings=deserialize_framebuffer_settings(get(message, "params", dict[str, Any])),
        variance=try_get(message, "variance", float),
    )


async def get_framebuffer_info(connection: Connection, id: FramebufferId) -> FramebufferInfo:
    result = await get_specific_object(connection, "Framebuffer", id)
    return deserialize_framebuffer_info(result)


async def update_framebuffer(
    connection: Connection, id: FramebufferId, image_operations: set[ImageOperationId]
) -> None:
    properties = {"imageOperations": list(image_operations)}
    await update_specific_object(connection, "Framebuffer", id, properties)


class Framebuffer:
    def __init__(self, id: FramebufferId, info: FramebufferInfo) -> None:
        self._id = id
        self._info = info

    @property
    def id(self) -> FramebufferId:
        return self._id

    @property
    def info(self) -> FramebufferInfo:
        return self._info

    @property
    def settings(self) -> FramebufferSettings:
        return self._info.settings

    @property
    def image_operations(self) -> set[ImageOperationId]:
        return self._info.settings.image_operations

    @image_operations.setter
    def image_operations(self, value: set[ImageOperationId]) -> None:
        self._info.settings.image_operations = value

    async def push(self, connection: Connection) -> None:
        await update_framebuffer(connection, self.id, self._info.settings.image_operations)

    async def pull(self, connection: Connection) -> None:
        self._info = await get_framebuffer_info(connection, self._id)


async def create_framebuffer(
    connection: Connection, settings: FramebufferSettings = FramebufferSettings()
) -> Framebuffer:
    params = serialize_framebuffer_settings(settings)
    id = await create_specific_object(connection, "Framebuffer", params)
    return Framebuffer(FramebufferId(id), FramebufferInfo(replace(settings)))


async def get_framebuffer(connection: Connection, id: FramebufferId) -> Framebuffer:
    info = await get_framebuffer_info(connection, id)
    return Framebuffer(id, info)
