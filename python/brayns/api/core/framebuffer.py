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

from collections.abc import Iterable
from dataclasses import dataclass, field
from enum import Enum
from typing import Any, NamedTuple

from brayns.network.connection import Connection
from brayns.utils.parsing import get, get_tuple, try_get

from .image_operation import ImageOperation
from .objects import Object, create_specific_object, get_specific_object, update_specific_object


class Framebuffer(Object): ...


class Resolution(NamedTuple):
    width: int
    height: int


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
    resolution: Resolution = Resolution(1920, 1080)
    format: FramebufferFormat = FramebufferFormat.SRGBA8
    channels: set[FramebufferChannel] = field(default_factory=lambda: {FramebufferChannel.COLOR})
    accumulation: Accumulation | None = None
    image_operations: list[ImageOperation] = field(default_factory=list)


def serialize_framebuffer_settings(settings: FramebufferSettings) -> dict[str, Any]:
    accumulation = settings.accumulation
    accumulation = None if accumulation is None else {"variance": accumulation.variance}

    return {
        "resolution": list(settings.resolution),
        "format": settings.format.value,
        "channels": [channel.value for channel in settings.channels],
        "accumulation": accumulation,
        "imageOperations": [operation.id for operation in settings.image_operations],
    }


def deserialize_framebuffer_settings(message: dict[str, Any]) -> FramebufferSettings:
    accumulation = try_get(message, "accumulation", dict[str, Any])
    accumulation = None if accumulation is None else Accumulation(get(accumulation, "variance", bool))

    return FramebufferSettings(
        resolution=Resolution(*get_tuple(message, "resolution", int, 2)),
        format=FramebufferFormat(get(message, "format", str)),
        channels={FramebufferChannel(value) for value in get(message, "channels", list[str])},
        accumulation=accumulation,
        image_operations=[ImageOperation(id) for id in get(message, "imageOperations", list[int])],
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


async def create_framebuffer(connection: Connection, settings: FramebufferSettings) -> Framebuffer:
    params = serialize_framebuffer_settings(settings)
    object = await create_specific_object(connection, "Framebuffer", params)
    return Framebuffer(object.id)


async def get_framebuffer(connection: Connection, framebuffer: Framebuffer) -> FramebufferInfo:
    result = await get_specific_object(connection, "Framebuffer", framebuffer)
    return deserialize_framebuffer_info(result)


async def update_framebuffer(
    connection: Connection, framebuffer: Framebuffer, image_operations: Iterable[ImageOperation]
) -> None:
    properties = {"imageOperations": [operation.id for operation in image_operations]}
    await update_specific_object(connection, "Framebuffer", framebuffer, properties)
