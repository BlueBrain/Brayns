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

import math
from abc import ABC
from dataclasses import dataclass, field, replace
from enum import Enum
from typing import Any, NewType, Protocol

from brayns.network.connection import Connection
from brayns.utils.box import Box2, Box3
from brayns.utils.parsing import deserialize_box, deserialize_vector, get, serialize_box, try_get
from brayns.utils.vector import Vector2, Vector3
from brayns.utils.view import View, Z

from .objects import create_composed_object, get_specific_object, update_specific_object

CameraId = NewType("CameraId", int)


@dataclass
class CameraSettings:
    view: View = field(default_factory=View)
    near_clip: float = 0.0
    image_region: Box2 = Box2(Vector2.zeros(), Vector2.ones())


def serialize_camera_settings(settings: CameraSettings) -> dict[str, Any]:
    return {
        "position": settings.view.position,
        "direction": settings.view.direction,
        "up": settings.view.up,
        "nearClip": settings.near_clip,
        "imageRegion": serialize_box(settings.image_region),
    }


def deserialize_camera_settings(message: dict[str, Any]) -> CameraSettings:
    return CameraSettings(
        view=View(
            position=deserialize_vector(message, "position", Vector3),
            direction=deserialize_vector(message, "direction", Vector3),
            up=deserialize_vector(message, "up", Vector3),
        ),
        near_clip=get(message, "nearClip", float),
        image_region=deserialize_box(get(message, "imageRegion", dict[str, Any]), Box2),
    )


async def create_camera(
    connection: Connection, typename: str, settings: CameraSettings, derived: dict[str, Any]
) -> CameraId:
    base = serialize_camera_settings(settings)
    id = await create_composed_object(connection, typename, base, derived)
    return CameraId(id)


async def get_camera_settings(connection: Connection, id: CameraId) -> CameraSettings:
    result = await get_specific_object(connection, "Camera", id)
    return deserialize_camera_settings(result)


async def update_camera_settings(connection: Connection, id: CameraId, settings: CameraSettings) -> None:
    properties = serialize_camera_settings(settings)
    await update_specific_object(connection, "Camera", id, properties)


class CameraProtocol(Protocol):
    async def push(self, connection: Connection, id: CameraId) -> None: ...
    async def pull(self, connection: Connection, id: CameraId) -> None: ...
    def look_at(self, target: Box3) -> float: ...


class Camera(ABC):
    def __init__(self, id: CameraId, settings: CameraSettings, protocol: CameraProtocol) -> None:
        self._id = id
        self._settings = settings
        self._protocol = protocol

    @property
    def id(self) -> CameraId:
        return self._id

    @property
    def settings(self) -> CameraSettings:
        return self._settings

    @settings.setter
    def settings(self, value: CameraSettings) -> None:
        self._settings = value

    @property
    def view(self) -> View:
        return self._settings.view

    @view.setter
    def view(self, value: View) -> None:
        self._settings.view = value

    async def push(self, connection: Connection) -> None:
        await update_camera_settings(connection, self._id, self._settings)
        await self._protocol.push(connection, self._id)

    async def pull(self, connection: Connection) -> None:
        self._settings = await get_camera_settings(connection, self._id)
        await self._protocol.pull(connection, self._id)

    def look_at(self, target: Box3) -> None:
        distance = self._protocol.look_at(target)
        distance = max(distance, self.settings.near_clip)
        distance += target.depth / 2
        self.view = View(position=distance * Z)


@dataclass
class DepthOfField:
    aperture_radius: float = 0.0
    focus_distance: float = 1.0


class StereoMode(Enum):
    NONE = "None"
    LEFT = "Left"
    RIGHT = "Right"
    SIDE_BY_SIDE = "SideBySide"
    TOP_BOTTOM = "TopBottom"


@dataclass
class Stereo:
    mode: StereoMode = StereoMode.NONE
    interpupillary_distance: float = 0.0635


@dataclass
class PerspectiveSettings:
    fovy: float = math.radians(45)
    depth_of_field: DepthOfField | None = None
    architectural: bool = False
    stereo: Stereo | None = None


def serialize_perspective_settings(settings: PerspectiveSettings) -> dict[str, Any]:
    depth_of_field = None

    if settings.depth_of_field is not None:
        depth_of_field = {
            "apertureRadius": settings.depth_of_field.aperture_radius,
            "focusDistance": settings.depth_of_field.focus_distance,
        }

    stereo = None

    if settings.stereo is not None:
        stereo = {
            "mode": settings.stereo.mode.value,
            "interpupillaryDistance": settings.stereo.interpupillary_distance,
        }

    return {
        "fovy": math.degrees(settings.fovy),
        "depthOfField": depth_of_field,
        "architectural": settings.architectural,
        "stereo": stereo,
    }


def deserialize_perspective_settings(message: dict[str, Any]) -> PerspectiveSettings:
    depth_of_field = try_get(message, "depthOfField", dict[str, Any])

    if depth_of_field is not None:
        depth_of_field = DepthOfField(
            aperture_radius=get(depth_of_field, "apertureRadius", float),
            focus_distance=get(depth_of_field, "focusDistance", float),
        )

    stereo = try_get(message, "stereo", dict[str, Any])

    if stereo is not None:
        stereo = Stereo(
            mode=StereoMode(get(stereo, "mode", str)),
            interpupillary_distance=get(stereo, "interpupillaryDistance", float),
        )

    return PerspectiveSettings(
        fovy=math.radians(get(message, "fovy", float)),
        depth_of_field=depth_of_field,
        architectural=get(message, "architectural", bool),
        stereo=stereo,
    )


async def get_perspective_settings(connection: Connection, id: CameraId) -> PerspectiveSettings:
    result = await get_specific_object(connection, "PerspectiveCamera", id)
    return deserialize_perspective_settings(result)


async def update_perspective_settings(connection: Connection, id: CameraId, settings: PerspectiveSettings) -> None:
    properties = serialize_perspective_settings(settings)
    await update_specific_object(connection, "PerspectiveCamera", id, properties)


def get_perspective_distance(fovy: float, target_height: float) -> float:
    return target_height / 2 / math.tan(fovy / 2)


@dataclass
class PerspectiveProtocol(CameraProtocol):
    settings: PerspectiveSettings

    async def push(self, connection: Connection, id: CameraId) -> None:
        await update_perspective_settings(connection, id, self.settings)

    async def pull(self, connection: Connection, id: CameraId) -> None:
        self.settings = await get_perspective_settings(connection, id)

    def look_at(self, target: Box3) -> float:
        return get_perspective_distance(self.settings.fovy, target.height)


class PerspectiveCamera(Camera):
    def __init__(self, id: CameraId, settings: CameraSettings, perspective: PerspectiveSettings) -> None:
        self._perspective = PerspectiveProtocol(perspective)
        super().__init__(id, settings, self._perspective)

    @property
    def perspective(self) -> PerspectiveSettings:
        return self._perspective.settings

    @perspective.setter
    def perspective(self, value: PerspectiveSettings) -> None:
        self._perspective.settings = value


async def create_perspective_camera(
    connection: Connection,
    settings: CameraSettings = CameraSettings(),
    perspective: PerspectiveSettings = PerspectiveSettings(),
) -> PerspectiveCamera:
    derived = serialize_perspective_settings(perspective)
    id = await create_camera(connection, "PerspectiveCamera", settings, derived)
    return PerspectiveCamera(id, replace(settings), replace(perspective))


async def get_perspective_camera(connection: Connection, id: CameraId) -> PerspectiveCamera:
    return PerspectiveCamera(
        id=id,
        settings=await get_camera_settings(connection, id),
        perspective=await get_perspective_settings(connection, id),
    )


@dataclass
class OrthographicSettings:
    height: float = 1.0


def serialize_orthographic_settings(value: OrthographicSettings) -> dict[str, Any]:
    return {"height": value.height}


def deserialize_orthographic_settings(message: dict[str, Any]) -> OrthographicSettings:
    return OrthographicSettings(height=get(message, "height", float))


async def get_orthographic_settings(connection: Connection, id: CameraId) -> OrthographicSettings:
    result = await get_specific_object(connection, "OrthographicCamera", id)
    return deserialize_orthographic_settings(result)


async def update_orthographic_settings(connection: Connection, id: CameraId, settings: OrthographicSettings) -> None:
    properties = serialize_orthographic_settings(settings)
    await update_specific_object(connection, "OrthographicCamera", id, properties)


@dataclass
class OrthographicProtocol(CameraProtocol):
    settings: OrthographicSettings

    async def push(self, connection: Connection, id: CameraId) -> None:
        await update_orthographic_settings(connection, id, self.settings)

    async def pull(self, connection: Connection, id: CameraId) -> None:
        self.settings = await get_orthographic_settings(connection, id)

    def look_at(self, target: Box3) -> float:
        self.settings.height = target.height
        return 0


class OrthographicCamera(Camera):
    def __init__(self, id: CameraId, settings: CameraSettings, orthographic: OrthographicSettings) -> None:
        self._orthographic = OrthographicProtocol(orthographic)
        super().__init__(id, settings, self._orthographic)

    @property
    def orthographic(self) -> OrthographicSettings:
        return self._orthographic.settings

    @orthographic.setter
    def orthographic(self, value: OrthographicSettings) -> None:
        self._orthographic.settings = value


async def create_orthographic_camera(
    connection: Connection,
    settings: CameraSettings = CameraSettings(),
    orthographic: OrthographicSettings = OrthographicSettings(),
) -> OrthographicCamera:
    derived = serialize_orthographic_settings(orthographic)
    id = await create_camera(connection, "OrthographicCamera", settings, derived)
    return OrthographicCamera(id, replace(settings), replace(orthographic))


async def get_orthographic_camera(connection: Connection, id: CameraId) -> OrthographicCamera:
    return OrthographicCamera(
        id=id,
        settings=await get_camera_settings(connection, id),
        orthographic=await get_orthographic_settings(connection, id),
    )
