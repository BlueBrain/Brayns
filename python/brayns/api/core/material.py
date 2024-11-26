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
from typing import Generic, TypedDict, TypeVar, Unpack

from brayns.network.connection import Connection
from brayns.utils.composing import serialize
from brayns.utils.parsing import deserialize

from .objects import CreateObjectParams, Object, create_specific_object, get_specific_object, update_specific_object
from .texture import Texture2D, VolumeTexture

T = TypeVar("T", float, tuple[float, float, float])


class Material(Object): ...


@dataclass
class MaterialTexture2D(Generic[T]):
    texture2d: Texture2D
    translation: tuple[float, float] = (0, 0)
    rotation: float = 0
    scale: tuple[float, float] = (1, 1)
    factor: T | None = None


@dataclass
class MaterialVolumeTexture(Generic[T]):
    volume_texture: VolumeTexture
    translation: tuple[float, float, float] = (0, 0, 0)
    rotation: tuple[float, float, float, float] = (0, 0, 0, 1)
    scale: tuple[float, float, float] = (0, 0, 0)
    factor: T | None = None


MaterialField = T | MaterialTexture2D[T] | MaterialVolumeTexture[T]

MaterialFloat = MaterialField[float]
MaterialColor3 = MaterialField[tuple[float, float, float]]


class AoMaterial(Material): ...


@dataclass
class GetAoMaterialResult:
    diffuse: MaterialColor3
    opacity: MaterialFloat


class UpdateAoMaterialParams(TypedDict, total=False):
    diffuse: MaterialColor3
    opacity: MaterialFloat


class CreateAoMaterialParams(CreateObjectParams, UpdateAoMaterialParams): ...


async def create_ao_material(connection: Connection, **settings: Unpack[CreateAoMaterialParams]) -> AoMaterial:
    object = await create_specific_object(connection, "AoMaterial", serialize(settings))
    return AoMaterial(object.id)


async def get_ao_material(connection: Connection, material: AoMaterial) -> GetAoMaterialResult:
    result = await get_specific_object(connection, "AoMaterial", material)
    return deserialize(result, GetAoMaterialResult)


async def update_ao_material(
    connection: Connection, material: AoMaterial, **settings: Unpack[UpdateAoMaterialParams]
) -> None:
    await update_specific_object(connection, "AoMaterial", material, serialize(settings))


class ScivisMaterial(Material): ...


@dataclass
class GetScivisMaterialResult:
    diffuse: MaterialColor3
    opacity: MaterialFloat
    specular: MaterialColor3
    shininess: MaterialFloat
    transparency_filter: tuple[float, float, float]


class UpdateScivisMaterialParams(TypedDict, total=False):
    diffuse: MaterialColor3
    opacity: MaterialFloat
    specular: MaterialColor3
    shininess: MaterialFloat
    transparency_filter: tuple[float, float, float]


class CreateScivisMaterialParams(CreateObjectParams, UpdateScivisMaterialParams): ...


async def create_scivis_material(
    connection: Connection, **settings: Unpack[CreateScivisMaterialParams]
) -> ScivisMaterial:
    object = await create_specific_object(connection, "ScivisMaterial", serialize(settings))
    return ScivisMaterial(object.id)


async def get_scivis_material(connection: Connection, material: ScivisMaterial) -> GetScivisMaterialResult:
    result = await get_specific_object(connection, "ScivisMaterial", material)
    return deserialize(result, GetScivisMaterialResult)


async def update_scivis_material(
    connection: Connection, material: ScivisMaterial, **settings: Unpack[UpdateScivisMaterialParams]
) -> None:
    await update_specific_object(connection, "ScivisMaterial", material, serialize(settings))


class PrincipledMaterial(Material): ...


@dataclass
class GetPrincipledMaterialResult:
    base_color: MaterialColor3
    edge_color: MaterialColor3
    metallic: MaterialFloat
    diffuse: MaterialFloat
    specular: MaterialFloat
    ior: MaterialFloat
    transmission: MaterialFloat
    transmission_color: MaterialColor3
    transmission_depth: MaterialFloat
    roughness: MaterialFloat
    anisotropy: MaterialFloat
    rotation: MaterialFloat
    normal: MaterialFloat
    base_normal: MaterialFloat
    thin: float
    thickness: MaterialFloat
    backlight: MaterialFloat
    coat: MaterialFloat
    coat_ior: MaterialFloat
    coat_color: MaterialColor3
    coat_thickness: MaterialFloat
    coat_roughness: MaterialFloat
    coat_normal: MaterialFloat
    sheen: MaterialFloat
    sheen_color: MaterialColor3
    sheen_tint: MaterialFloat
    sheen_roughness: MaterialFloat
    opacity: MaterialFloat
    emissive_color: MaterialColor3


class UpdatePrincipledMaterialParams(TypedDict, total=False):
    base_color: MaterialColor3
    edge_color: MaterialColor3
    metallic: MaterialFloat
    diffuse: MaterialFloat
    specular: MaterialFloat
    ior: MaterialFloat
    transmission: MaterialFloat
    transmission_color: MaterialColor3
    transmission_depth: MaterialFloat
    roughness: MaterialFloat
    anisotropy: MaterialFloat
    rotation: MaterialFloat
    normal: MaterialFloat
    base_normal: MaterialFloat
    thin: float
    thickness: MaterialFloat
    backlight: MaterialFloat
    coat: MaterialFloat
    coat_ior: MaterialFloat
    coat_color: MaterialColor3
    coat_thickness: MaterialFloat
    coat_roughness: MaterialFloat
    coat_normal: MaterialFloat
    sheen: MaterialFloat
    sheen_color: MaterialColor3
    sheen_tint: MaterialFloat
    sheen_roughness: MaterialFloat
    opacity: MaterialFloat
    emissive_color: MaterialColor3


class CreatePrincipledMaterialParams(CreateObjectParams, UpdatePrincipledMaterialParams): ...


async def create_principled_material(
    connection: Connection, **settings: Unpack[CreatePrincipledMaterialParams]
) -> PrincipledMaterial:
    object = await create_specific_object(connection, "PrincipledMaterial", serialize(settings))
    return PrincipledMaterial(object.id)


async def get_principled_material(connection: Connection, material: PrincipledMaterial) -> GetPrincipledMaterialResult:
    result = await get_specific_object(connection, "PrincipledMaterial", material)
    return deserialize(result, GetPrincipledMaterialResult)


async def update_principled_material(
    connection: Connection, material: PrincipledMaterial, **settings: Unpack[UpdatePrincipledMaterialParams]
) -> None:
    await update_specific_object(connection, "PrincipledMaterial", material, serialize(settings))
