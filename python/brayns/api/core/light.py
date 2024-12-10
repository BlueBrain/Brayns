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
from typing import Literal, TypedDict, Unpack

from brayns.network.connection import Connection
from brayns.utils.composing import serialize
from brayns.utils.parsing import deserialize

from .objects import (
    CreateObjectParams,
    Object,
    create_specific_object,
    get_specific_object,
    update_specific_object,
)
from .texture import Texture2D


class Light(Object): ...


@dataclass
class GetLightResult:
    color: tuple[float, float, float]
    visible: bool


class UpdateLightParams(TypedDict, total=False):
    color: tuple[float, float, float]
    visible: bool


class CreateLightParams(CreateObjectParams, UpdateLightParams): ...


async def get_light(connection: Connection, light: Light) -> GetLightResult:
    result = await get_specific_object(connection, "Light", light)
    return deserialize(result, GetLightResult)


async def update_light(connection: Connection, light: Light, **settings: Unpack[UpdateLightParams]) -> None:
    await update_specific_object(connection, "Light", light, serialize(settings))


@dataclass
class Power:
    value: float = 1.0
    type: Literal["Power"] = "Power"


@dataclass
class Intensity:
    value: float = 1.0
    type: Literal["Intensity"] = "Intensity"


@dataclass
class Radiance:
    value: float = 1.0
    type: Literal["Radiance"] = "Radiance"


@dataclass
class Irradiance:
    value: float = 1.0
    type: Literal["Irradiance"] = "Irradiance"


class DistantLight(Light): ...


@dataclass
class GetDistantLightResult:
    direction: tuple[float, float, float]
    angular_diameter: float
    intensity: Radiance | Irradiance


class UpdateDistantLightParams(TypedDict, total=False):
    direction: tuple[float, float, float]
    angular_diameter: float
    intensity: Radiance | Irradiance


class CreateDistantLightParams(CreateLightParams, UpdateDistantLightParams): ...


async def create_distant_light(connection: Connection, **settings: Unpack[CreateDistantLightParams]) -> DistantLight:
    object = await create_specific_object(connection, "DistantLight", serialize(settings))
    return DistantLight(object.id)


async def get_distant_light(connection: Connection, light: DistantLight) -> GetDistantLightResult:
    result = await get_specific_object(connection, "DistantLight", light)
    return deserialize(result, GetDistantLightResult)


async def update_distant_light(
    connection: Connection, light: DistantLight, **settings: Unpack[UpdateDistantLightParams]
) -> None:
    await update_specific_object(connection, "DistantLight", light, serialize(settings))


class SphereLight(Light): ...


@dataclass
class GetSphereLightResult:
    position: tuple[float, float, float]
    radius: float
    intensity: Power | Intensity | Radiance


class UpdateSphereLightParams(TypedDict, total=False):
    position: tuple[float, float, float]
    radius: float
    intensity: Power | Intensity | Radiance


class CreateSphereLightParams(CreateLightParams, UpdateSphereLightParams): ...


async def create_sphere_light(connection: Connection, **settings: Unpack[CreateSphereLightParams]) -> SphereLight:
    object = await create_specific_object(connection, "SphereLight", serialize(settings))
    return SphereLight(object.id)


async def get_sphere_light(connection: Connection, light: SphereLight) -> GetSphereLightResult:
    result = await get_specific_object(connection, "SphereLight", light)
    return deserialize(result, GetSphereLightResult)


async def update_sphere_light(
    connection: Connection, light: SphereLight, **settings: Unpack[UpdateSphereLightParams]
) -> None:
    await update_specific_object(connection, "SphereLight", light, serialize(settings))


class SpotLight(Light): ...


@dataclass
class GetSpotLightResult:
    position: tuple[float, float, float]
    direction: tuple[float, float, float]
    opening_angle: float
    penumbra_angle: float
    outer_radius: float
    inner_radius: float
    intensity: Power | Intensity | Radiance


class UpdateSpotLightParams(TypedDict, total=False):
    position: tuple[float, float, float]
    direction: tuple[float, float, float]
    opening_angle: float
    penumbra_angle: float
    outer_radius: float
    inner_radius: float
    intensity: Power | Intensity | Radiance


class CreateSpotLightParams(CreateLightParams, UpdateSpotLightParams): ...


async def create_spot_light(connection: Connection, **settings: Unpack[CreateSpotLightParams]) -> SpotLight:
    object = await create_specific_object(connection, "SpotLight", serialize(settings))
    return SpotLight(object.id)


async def get_spot_light(connection: Connection, light: SpotLight) -> GetSpotLightResult:
    result = await get_specific_object(connection, "SpotLight", light)
    return deserialize(result, GetSpotLightResult)


async def update_spot_light(
    connection: Connection, light: SpotLight, **settings: Unpack[UpdateSpotLightParams]
) -> None:
    await update_specific_object(connection, "SpotLight", light, serialize(settings))


class QuadLight(Light): ...


@dataclass
class GetQuadLightResult:
    position: tuple[float, float, float]
    edge1: tuple[float, float, float]
    edge2: tuple[float, float, float]
    intensity: Power | Intensity | Radiance


class UpdateQuadLightParams(TypedDict, total=False):
    position: tuple[float, float, float]
    edge1: tuple[float, float, float]
    edge2: tuple[float, float, float]
    intensity: Power | Intensity | Radiance


class CreateQuadLightParams(CreateLightParams, UpdateQuadLightParams): ...


async def create_quad_light(connection: Connection, **settings: Unpack[CreateQuadLightParams]) -> QuadLight:
    object = await create_specific_object(connection, "QuadLight", serialize(settings))
    return QuadLight(object.id)


async def get_quad_light(connection: Connection, light: QuadLight) -> GetQuadLightResult:
    result = await get_specific_object(connection, "QuadLight", light)
    return deserialize(result, GetQuadLightResult)


async def update_quad_light(
    connection: Connection, light: QuadLight, **settings: Unpack[UpdateQuadLightParams]
) -> None:
    await update_specific_object(connection, "QuadLight", light, serialize(settings))


class CylinderLight(Light): ...


@dataclass
class GetCylinderLightResult:
    start: tuple[float, float, float]
    end: tuple[float, float, float]
    radius: float
    intensity: Power | Intensity | Radiance


class UpdateCylinderLightParams(TypedDict, total=False):
    start: tuple[float, float, float]
    end: tuple[float, float, float]
    radius: float
    intensity: Power | Intensity | Radiance


class CreateCylinderLightParams(CreateLightParams, UpdateCylinderLightParams): ...


async def create_cylinder_light(
    connection: Connection, **settings: Unpack[CreateCylinderLightParams]
) -> CylinderLight:
    object = await create_specific_object(connection, "CylinderLight", serialize(settings))
    return CylinderLight(object.id)


async def get_cylinder_light(connection: Connection, light: CylinderLight) -> GetCylinderLightResult:
    result = await get_specific_object(connection, "CylinderLight", light)
    return deserialize(result, GetCylinderLightResult)


async def update_cylinder_light(
    connection: Connection, light: CylinderLight, **settings: Unpack[UpdateCylinderLightParams]
) -> None:
    await update_specific_object(connection, "CylinderLight", light, serialize(settings))


class HdriLight(Light): ...


@dataclass
class GetHdriLightResult:
    map: Texture2D
    up: tuple[float, float, float]
    direction: tuple[float, float, float]
    intensity_scale: float


class UpdateHdriLightParams(TypedDict, total=False):
    map: Texture2D
    up: tuple[float, float, float]
    direction: tuple[float, float, float]
    intensity_scale: float


class CreateHdriLightParams(CreateLightParams, UpdateHdriLightParams):
    map: Texture2D


async def create_hdri_light(connection: Connection, **settings: Unpack[CreateHdriLightParams]) -> HdriLight:
    object = await create_specific_object(connection, "HdriLight", serialize(settings))
    return HdriLight(object.id)


async def get_hdri_light(connection: Connection, light: HdriLight) -> GetHdriLightResult:
    result = await get_specific_object(connection, "HdriLight", light)
    return deserialize(result, GetHdriLightResult)


async def update_hdri_light(
    connection: Connection, light: HdriLight, **settings: Unpack[UpdateHdriLightParams]
) -> None:
    await update_specific_object(connection, "HdriLight", light, serialize(settings))


class AmbientLight(Light): ...


@dataclass
class GetAmbientLightResult:
    intensity: Radiance | Irradiance


class UpdateAmbientLightParams(TypedDict, total=False):
    intensity: Radiance | Irradiance


class CreateAmbientLightParams(CreateLightParams, UpdateAmbientLightParams): ...


async def create_ambient_light(connection: Connection, **settings: Unpack[CreateAmbientLightParams]) -> AmbientLight:
    object = await create_specific_object(connection, "AmbientLight", serialize(settings))
    return AmbientLight(object.id)


async def get_ambient_light(connection: Connection, light: AmbientLight) -> GetAmbientLightResult:
    result = await get_specific_object(connection, "AmbientLight", light)
    return deserialize(result, GetAmbientLightResult)


async def update_ambient_light(
    connection: Connection, light: AmbientLight, **settings: Unpack[UpdateAmbientLightParams]
) -> None:
    await update_specific_object(connection, "AmbientLight", light, serialize(settings))


class SunSkyLight(Light): ...


@dataclass
class GetSunSkyLightResult:
    up: tuple[float, float, float]
    direction: tuple[float, float, float]
    turbidity: float
    albedo: float
    horizon_extension: float
    intensity_scale: float


class UpdateSunSkyLightParams(TypedDict, total=False):
    up: tuple[float, float, float]
    direction: tuple[float, float, float]
    turbidity: float
    albedo: float
    horizon_extension: float
    intensity_scale: float


class CreateSunSkyLightParams(CreateLightParams, UpdateSunSkyLightParams): ...


async def create_sunsky_light(connection: Connection, **settings: Unpack[CreateSunSkyLightParams]) -> SunSkyLight:
    object = await create_specific_object(connection, "SunSkyLight", serialize(settings))
    return SunSkyLight(object.id)


async def get_sunsky_light(connection: Connection, light: SunSkyLight) -> GetSunSkyLightResult:
    result = await get_specific_object(connection, "SunSkyLight", light)
    return deserialize(result, GetSunSkyLightResult)


async def update_sunsky_light(
    connection: Connection, light: SunSkyLight, **settings: Unpack[UpdateSunSkyLightParams]
) -> None:
    await update_specific_object(connection, "SunSkyLight", light, serialize(settings))
