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

from dataclasses import dataclass, field
from enum import Enum
from typing import Any, NamedTuple

from brayns.network.connection import Connection
from brayns.utils.parsing import deserialize_vector, get, get_tuple, try_get
from brayns.utils.vector import Vector3

from .objects import Object, create_composed_object, get_specific_object, update_specific_object


class Volume(Object): ...


class VoxelType(Enum):
    U8 = "U8"
    U16 = "U16"
    F32 = "F32"
    F64 = "F64"


class VolumeFilter(Enum):
    NEAREST = "Nearest"
    LINEAR = "Linear"
    CUBIC = "Cubic"


class VolumeType(Enum):
    CELL_CENTERED = "CellCentered"
    VERTEX_CENTERED = "VertexCentered"


@dataclass
class RegularVolumeUpdate:
    origin: Vector3 = Vector3()
    spacing: Vector3 = Vector3.full(1.0)
    type: VolumeType = VolumeType.VERTEX_CENTERED
    filter: VolumeFilter = VolumeFilter.LINEAR
    background: float | None = None


def serialize_volume_update(settings: RegularVolumeUpdate) -> dict[str, Any]:
    return {
        "origin": list(settings.origin),
        "spacing": list(settings.spacing),
        "type": settings.type.value,
        "filter": settings.filter.value,
        "background": settings.background,
    }


def deserialize_volume_update(message: dict[str, Any]) -> RegularVolumeUpdate:
    return RegularVolumeUpdate(
        origin=deserialize_vector(message, "origin", Vector3),
        spacing=deserialize_vector(message, "spacing", Vector3),
        type=VolumeType(get(message, "type", str)),
        filter=VolumeFilter(get(message, "filter", str)),
        background=try_get(message, "background", float | None, None),
    )


class Size3(NamedTuple):
    x: int
    y: int
    z: int


@dataclass
class RegularVolumeSettings:
    voxel_type: VoxelType
    voxel_count: Size3
    update: RegularVolumeUpdate = field(default_factory=RegularVolumeUpdate)


def serialize_volume_settings(settings: RegularVolumeSettings) -> dict[str, Any]:
    return {
        "voxelType": settings.voxel_type.value,
        "voxelCount": list(settings.voxel_count),
        "settings": serialize_volume_update(settings.update),
    }


def deserialize_volume_settings(message: dict[str, Any]) -> RegularVolumeSettings:
    return RegularVolumeSettings(
        voxel_type=VoxelType(get(message, "voxelType", str)),
        voxel_count=Size3(*get_tuple(message, "voxelCount", int, 3)),
        update=deserialize_volume_update(get(message, "settings", dict[str, Any])),
    )


class RegularVolume(Volume): ...


async def create_regular_volume(connection: Connection, settings: RegularVolumeSettings, data: bytes) -> RegularVolume:
    params = serialize_volume_settings(settings)
    object = await create_composed_object(connection, "RegularVolume", None, params, data)
    return RegularVolume(object.id)


async def get_regular_volume(connection: Connection, volume: RegularVolume) -> RegularVolumeSettings:
    result = await get_specific_object(connection, "RegularVolume", volume)
    return deserialize_volume_settings(result)


async def update_regular_volume(connection: Connection, volume: RegularVolume, settings: RegularVolumeUpdate) -> None:
    params = serialize_volume_update(settings)
    await update_specific_object(connection, "RegularVolume", volume, params)
