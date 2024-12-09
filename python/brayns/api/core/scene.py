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
from typing import Required, TypedDict, Unpack

from brayns.network.connection import Connection
from brayns.utils.composing import serialize
from brayns.utils.parsing import deserialize

from .geometry import Geometry
from .light import Light
from .objects import CreateObjectParams, Object, create_specific_object, get_specific_object
from .transfer_function import TransferFunction
from .volume import Volume


class GeometricModel(Object): ...


@dataclass
class GetGeometricModelResult:
    geometry: Geometry
    materials: list[int]
    colors: list[tuple[float, float, float, float]]
    indices: list[int]
    invert_normals: bool
    id: int


class UpdateGeometricModelParams(TypedDict, total=False):
    geometry: Geometry
    materials: list[int]
    colors: list[tuple[float, float, float, float]]
    indices: list[int]
    invert_normals: bool
    id: int


class CreateGeometricModelParams(CreateObjectParams, UpdateGeometricModelParams):
    geometry: Required[Geometry]


async def create_geometric_model(
    connection: Connection, **settings: Unpack[CreateGeometricModelParams]
) -> GeometricModel:
    object = await create_specific_object(connection, "GeometricModel", serialize(settings))
    return GeometricModel(object.id)


async def get_geometric_model(connection: Connection, model: GeometricModel) -> GetGeometricModelResult:
    result = await get_specific_object(connection, "GeometricModel", model)
    return deserialize(result, GetGeometricModelResult)


class VolumetricModel(Object): ...


@dataclass
class GetVolumetricModelResult:
    volume: Volume
    transfer_function: TransferFunction
    density_scale: float
    anisotropy: float
    id: int


class UpdateVolumetricModelParams(TypedDict, total=False):
    volume: Volume
    transfer_function: TransferFunction
    density_scale: float
    anisotropy: float
    id: int


class CreateVolumetricModelParams(CreateObjectParams, UpdateVolumetricModelParams):
    volume: Required[Volume]
    transfer_function: Required[TransferFunction]


async def create_volumetric_model(
    connection: Connection, **settings: Unpack[CreateVolumetricModelParams]
) -> VolumetricModel:
    object = await create_specific_object(connection, "VolumetricModel", serialize(settings))
    return VolumetricModel(object.id)


async def get_volumetric_model(connection: Connection, model: VolumetricModel) -> GetVolumetricModelResult:
    result = await get_specific_object(connection, "VolumetricModel", model)
    return deserialize(result, GetVolumetricModelResult)


class Group(Object): ...


@dataclass
class GetGroupResult:
    geometries: list[GeometricModel]
    clipping_geometries: list[GeometricModel]
    volumes: list[VolumetricModel]
    lights: list[Light]
    bounds: tuple[tuple[float, float, float], tuple[float, float, float]]


class UpdateGroupParams(TypedDict, total=False):
    geometries: list[GeometricModel]
    clipping_geometries: list[GeometricModel]
    volumes: list[VolumetricModel]
    lights: list[Light]


class CreateGroupParams(CreateObjectParams, UpdateGroupParams): ...


async def create_group(connection: Connection, **settings: Unpack[CreateGroupParams]) -> Group:
    object = await create_specific_object(connection, "Group", serialize(settings))
    return Group(object.id)


async def get_group(connection: Connection, group: Group) -> GetGroupResult:
    result = await get_specific_object(connection, "Group", group)
    return deserialize(result, GetGroupResult)


class Instance(Object): ...


@dataclass
class GetInstanceResult:
    group: Group
    translation: tuple[float, float, float]
    rotation: tuple[float, float, float, float]
    scale: tuple[float, float, float]
    id: int
    bounds: tuple[tuple[float, float, float], tuple[float, float, float]]


class UpdateInstanceParams(TypedDict, total=False):
    group: Group
    translation: tuple[float, float, float]
    rotation: tuple[float, float, float, float]
    scale: tuple[float, float, float]
    id: int


class CreateInstanceParams(CreateObjectParams, UpdateInstanceParams):
    group: Required[Group]


async def create_instance(connection: Connection, **settings: Unpack[CreateInstanceParams]) -> Instance:
    object = await create_specific_object(connection, "Instance", serialize(settings))
    return Instance(object.id)


async def get_instance(connection: Connection, instance: Instance) -> GetInstanceResult:
    result = await get_specific_object(connection, "Instance", instance)
    return deserialize(result, GetInstanceResult)


class World(Object): ...


@dataclass
class GetWorldResult:
    instances: list[Instance]
    bounds: tuple[tuple[float, float, float], tuple[float, float, float]]


class UpdateWorldParams(TypedDict, total=False):
    instances: list[Instance]


class CreateWorldParams(CreateObjectParams, UpdateWorldParams): ...


async def create_world(connection: Connection, **settings: Unpack[CreateWorldParams]) -> World:
    object = await create_specific_object(connection, "World", serialize(settings))
    return World(object.id)


async def get_world(connection: Connection, world: World) -> GetWorldResult:
    result = await get_specific_object(connection, "World", world)
    return deserialize(result, GetWorldResult)
