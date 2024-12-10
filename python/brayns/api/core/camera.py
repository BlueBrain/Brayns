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

from .objects import (
    CreateObjectParams,
    Object,
    create_specific_object,
    get_specific_object,
    update_specific_object,
)


class Camera(Object): ...


@dataclass
class GetCameraResult:
    position: tuple[float, float, float]
    direction: tuple[float, float, float]
    up: tuple[float, float, float]
    near_clip: float
    region: tuple[tuple[float, float], tuple[float, float]]


class UpdateCameraParams(TypedDict, total=False):
    position: tuple[float, float, float]
    direction: tuple[float, float, float]
    up: tuple[float, float, float]
    near_clip: float
    region: tuple[tuple[float, float], tuple[float, float]]


class CreateCameraParams(CreateObjectParams, UpdateCameraParams): ...


async def get_camera(connection: Connection, camera: Camera) -> GetCameraResult:
    result = await get_specific_object(connection, "Camera", camera)
    return deserialize(result, GetCameraResult)


async def update_camera(connection: Connection, camera: Camera, **settings: Unpack[UpdateCameraParams]) -> None:
    await update_specific_object(connection, "Camera", camera, serialize(settings))


class PerspectiveCamera(Camera): ...


class Stereo(Enum):
    NONE = "None"
    LEFT = "Left"
    RIGHT = "Right"
    SIDE_BY_SIDE = "SideBySide"
    TOP_BOTTOM = "TopBottom"


@dataclass
class GetPerspectiveCameraResult:
    fovy: float
    aspect: float
    aperture_radius: float
    focus_distance: float
    architectural: bool
    stereo: Stereo
    interpupillary_distance: float


class UpdatePerspectiveCameraParams(TypedDict, total=False):
    fovy: float
    aspect: float
    aperture_radius: float
    focus_distance: float
    architectural: bool
    stereo: Stereo
    interpupillary_distance: float


class CreatePerspectiveCameraParams(CreateCameraParams, UpdatePerspectiveCameraParams): ...


async def create_perspective_camera(
    connection: Connection, **settings: Unpack[CreatePerspectiveCameraParams]
) -> PerspectiveCamera:
    object = await create_specific_object(connection, "PerspectiveCamera", serialize(settings))
    return PerspectiveCamera(object.id)


async def get_perspective_camera(connection: Connection, camera: PerspectiveCamera) -> GetPerspectiveCameraResult:
    result = await get_specific_object(connection, "PerspectiveCamera", camera)
    return deserialize(result, GetPerspectiveCameraResult)


async def update_perspective_camera(
    connection: Connection, camera: PerspectiveCamera, **settings: Unpack[UpdatePerspectiveCameraParams]
) -> None:
    await update_specific_object(connection, "PerspectiveCamera", camera, serialize(settings))


class OrthographicCamera(Camera): ...


@dataclass
class GetOrthographicCameraResult:
    height: float
    aspect: float


class UpdateOrthographicCameraParams(TypedDict, total=False):
    height: float
    aspect: float


class CreateOrthographicCameraParams(CreateCameraParams, UpdateOrthographicCameraParams): ...


async def create_orthographic_camera(
    connection: Connection, **settings: Unpack[CreateOrthographicCameraParams]
) -> OrthographicCamera:
    object = await create_specific_object(connection, "OrthographicCamera", serialize(settings))
    return OrthographicCamera(object.id)


async def get_orthographic_camera(connection: Connection, camera: OrthographicCamera) -> GetOrthographicCameraResult:
    result = await get_specific_object(connection, "OrthographicCamera", camera)
    return deserialize(result, GetOrthographicCameraResult)


async def update_orthographic_camera(
    connection: Connection, camera: OrthographicCamera, **settings: Unpack[UpdateOrthographicCameraParams]
) -> None:
    await update_specific_object(connection, "OrthographicCamera", camera, serialize(settings))


class PanoramicCamera(Camera): ...


@dataclass
class GetPanoramicCameraResult:
    stereo: Stereo
    interpupillary_distance: float


class UpdatePanoramicCameraParams(TypedDict, total=False):
    stereo: Stereo
    interpupillary_distance: float


class CreatePanoramicCameraParams(CreateCameraParams, UpdatePanoramicCameraParams): ...


async def create_panoramic_camera(
    connection: Connection, **settings: Unpack[CreatePanoramicCameraParams]
) -> PanoramicCamera:
    object = await create_specific_object(connection, "PanoramicCamera", serialize(settings))
    return PanoramicCamera(object.id)


async def get_panoramic_camera(connection: Connection, camera: PanoramicCamera) -> GetPanoramicCameraResult:
    result = await get_specific_object(connection, "PanoramicCamera", camera)
    return deserialize(result, GetPanoramicCameraResult)


async def update_panoramic_camera(
    connection: Connection, camera: PanoramicCamera, **settings: Unpack[UpdatePanoramicCameraParams]
) -> None:
    await update_specific_object(connection, "PanoramicCamera", camera, serialize(settings))
