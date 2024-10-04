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

from dataclasses import dataclass
from enum import Enum
from typing import TypedDict, Unpack

from brayns.network.connection import Connection
from brayns.utils.composing import serialize
from brayns.utils.parsing import deserialize

from .image_operation import ImageOperation
from .objects import (
    CreateObjectParams,
    Object,
    create_specific_object,
    get_specific_object,
    update_specific_object,
)


class Framebuffer(Object): ...


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
class GetFramebufferResult:
    resolution: tuple[int, int]
    format: FramebufferFormat
    channels: set[FramebufferChannel]
    accumulation: bool
    variance: bool
    operations: list[ImageOperation]
    variance_estimate: float | None


class UpdateFramebufferParams(TypedDict, total=False):
    resolution: tuple[int, int]
    format: FramebufferFormat
    channels: set[FramebufferChannel]
    accumulation: bool
    variance: bool
    operations: list[ImageOperation]


class CreateFramebufferParams(CreateObjectParams, UpdateFramebufferParams): ...


async def create_framebuffer(connection: Connection, **settings: Unpack[CreateFramebufferParams]) -> Framebuffer:
    object = await create_specific_object(connection, "Framebuffer", serialize(settings))
    return Framebuffer(object.id)


async def get_framebuffer(connection: Connection, framebuffer: Framebuffer) -> GetFramebufferResult:
    result = await get_specific_object(connection, "Framebuffer", framebuffer)
    return deserialize(result, GetFramebufferResult)


async def update_framebuffer(
    connection: Connection, framebuffer: Framebuffer, **settings: Unpack[UpdateFramebufferParams]
) -> None:
    await update_specific_object(connection, "Framebuffer", framebuffer, serialize(settings))


async def clear_framebuffer(connection: Connection, framebuffer: Framebuffer) -> None:
    await connection.get_result("clearFramebuffer", {"id": framebuffer.id})
