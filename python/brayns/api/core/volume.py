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


class Volume(Object): ...


class RegularVolume(Volume): ...


class VoxelType(Enum):
    U8 = "U8"
    U16 = "U16"
    F32 = "F32"
    F64 = "F64"


class VolumeFilter(Enum):
    NEAREST = "Nearest"
    LINEAR = "Linear"
    CUBIC = "Cubic"


@dataclass
class GetRegularVolumeResult:
    voxel_type: VoxelType
    size: tuple[int, int, int]
    origin: tuple[float, float, float]
    spacing: tuple[float, float, float]
    cell_centered: bool
    filter: VolumeFilter
    background: float | None


class UpdateRegularVolumeParams(TypedDict, total=False):
    origin: tuple[float, float, float]
    spacing: tuple[float, float, float]
    cell_centered: bool
    filter: VolumeFilter
    background: float | None


class CreateRegularVolumeParams(CreateObjectParams, UpdateRegularVolumeParams):
    voxel_type: VoxelType
    size: tuple[int, int, int]


async def create_regular_volume(
    connection: Connection, data: bytes, **settings: Unpack[CreateRegularVolumeParams]
) -> RegularVolume:
    object = await create_specific_object(connection, "RegularVolume", serialize(settings), data)
    return RegularVolume(object.id)


async def get_regular_volume(connection: Connection, volume: RegularVolume) -> GetRegularVolumeResult:
    result = await get_specific_object(connection, "RegularVolume", volume)
    return deserialize(result, GetRegularVolumeResult)


async def update_regular_volume(
    connection: Connection, volume: RegularVolume, **settings: Unpack[UpdateRegularVolumeParams]
) -> None:
    await update_specific_object(connection, "RegularVolume", volume, serialize(settings))
