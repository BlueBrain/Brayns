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

from .objects import CreateObjectParams, Object, create_specific_object, get_specific_object, update_specific_object
from .transfer_function import TransferFunction
from .volume import Volume


class Texture(Object): ...


class Texture2D(Texture): ...


class TextureFormat(Enum):
    RGBA8 = "Rgba8"
    SRGBA8 = "Srgba8"
    RGBA32F = "Rgba32F"
    RGB8 = "Rgb8"
    SRGB8 = "Srgb8"
    RGB32F = "Rgb32F"
    R8 = "R8"
    RA8 = "Ra8"
    L8 = "L8"
    LA8 = "La8"
    R32F = "R32F"
    RGBA16 = "Rgba16"
    RGB16 = "Rgb16"
    RA16 = "Ra16"
    R16 = "R16"


class TextureFilter(Enum):
    LINEAR = "Linear"
    NEAREST = "Nearest"


class TextureWrap(Enum):
    REPEAT = "Repeat"
    MORRORED_REPEAT = "MirroredRepeat"
    CLAMP_TO_EDGE = "ClampToEdge"


@dataclass
class GetTexture2DResult:
    format: TextureFormat
    size: tuple[int, int]
    filter: TextureFilter
    wrap: TextureWrap


class UpdateTexture2DParams(TypedDict, total=False):
    filter: TextureFilter
    wrap: TextureWrap


class CreateTexture2DParams(CreateObjectParams, UpdateTexture2DParams):
    format: TextureFormat
    size: tuple[int, int]


async def create_texture2d(
    connection: Connection, data: bytes, **settings: Unpack[CreateTexture2DParams]
) -> Texture2D:
    object = await create_specific_object(connection, "Texture2D", serialize(settings), data)
    return Texture2D(object.id)


async def get_texture2d(connection: Connection, texture: Texture2D) -> GetTexture2DResult:
    result = await get_specific_object(connection, "Texture2D", texture)
    return deserialize(result, GetTexture2DResult)


async def update_texture2d(
    connection: Connection, texture: Texture2D, **settings: Unpack[UpdateTexture2DParams]
) -> None:
    await update_specific_object(connection, "Texture2D", texture, serialize(settings))


class VolumeTexture(Texture): ...


@dataclass
class GetVolumeTextureResult:
    volume: Volume
    transfer_function: TransferFunction


class UpdateVolumeTextureParams(TypedDict):
    volume: Volume
    transfer_function: TransferFunction


class CreateVolumeTextureParams(CreateObjectParams, UpdateVolumeTextureParams): ...


async def create_volume_texture(
    connection: Connection, **settings: Unpack[CreateVolumeTextureParams]
) -> VolumeTexture:
    object = await create_specific_object(connection, "VolumeTexture", serialize(settings))
    return VolumeTexture(object.id)


async def get_volume_texture(connection: Connection, texture: VolumeTexture) -> GetVolumeTextureResult:
    result = await get_specific_object(connection, "VolumeTexture", texture)
    return deserialize(result, GetVolumeTextureResult)


async def update_volume_texture(
    connection: Connection, texture: VolumeTexture, **settings: Unpack[UpdateVolumeTextureParams]
) -> None:
    await update_specific_object(connection, "VolumeTexture", texture, serialize(settings))
