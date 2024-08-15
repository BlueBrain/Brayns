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
from brayns.utils.parsing import get, get_tuple

from .image_operation import ImageOperationId
from .objects import create_specific_object, get_specific_object

FramebufferId = NewType("FramebufferId", int)


class FramebufferFormat(Enum):
    RGBA8 = "rgba8"
    SRGBA8 = "srgba8"
    RGBA32F = "rgba32F"


class FramebufferChannel(Enum):
    COLOR = "color"
    DEPTH = "depth"
    NORMAL = "normal"
    ALBEDO = "albedo"
    PRIMITIVE_ID = "primitive_id"
    MODEL_ID = "model_id"
    INSTANCE_ID = "instance_id"


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
    child = None if accumulation is None else {"variance": accumulation.variance}

    return {
        "resolution": list(settings.resolution),
        "format": settings.format.value,
        "channels": [channel.value for channel in settings.channels],
        "accumulation": child,
        "image_operations": list(settings.image_operations),
    }


def deserialize_framebuffer_settings(message: dict[str, Any]) -> FramebufferSettings:
    child = message.get("accumulation")
    accumulation = None if child is None else Accumulation(get(child, "variance", bool))

    return FramebufferSettings(
        resolution=tuple(get_tuple(message, "resolution", int, 2)),
        format=FramebufferFormat(get(message, "format", str)),
        channels={FramebufferChannel(value) for value in get(message, "channels", list[str])},
        accumulation=accumulation,
        image_operations=set(get(message, "image_operations", list[int])),
    )


async def get_framebuffer_settings(connection: Connection, id: FramebufferId) -> FramebufferSettings:
    result = await get_specific_object(connection, "framebuffer", id)
    return deserialize_framebuffer_settings(result)


class Framebuffer:
    def __init__(self, id: FramebufferId, settings: FramebufferSettings) -> None:
        self._id = id
        self._settings = settings

    @property
    def id(self) -> FramebufferId:
        return self._id

    @property
    def settings(self) -> FramebufferSettings:
        return self._settings


async def create_framebuffer(
    connection: Connection, settings: FramebufferSettings = FramebufferSettings()
) -> Framebuffer:
    params = serialize_framebuffer_settings(settings)
    id = await create_specific_object(connection, "framebuffer", params)
    return Framebuffer(FramebufferId(id), replace(settings))


async def get_framebuffer(connection: Connection, id: FramebufferId) -> Framebuffer:
    settings = await get_framebuffer_settings(connection, id)
    return Framebuffer(id, settings)