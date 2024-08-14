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
from typing import Any, NewType, Protocol

from brayns.network.connection import Connection
from brayns.utils.box import Box3
from brayns.utils.parsing import get
from brayns.utils.vector import Vector3
from brayns.utils.view import View, Z

from .objects import create_composed_object, get_specific_object, update_specific_object

CameraId = NewType("CameraId", int)


@dataclass
class CameraSettings:
    view: View = field(default_factory=View)
    near_clip: float = 0.0


def serialize_camera_settings(value: CameraSettings) -> dict[str, Any]:
    return {
        "position": value.view.position,
        "direction": value.view.direction,
        "up": value.view.up,
        "near_clip": value.near_clip,
    }


def deserialize_camera_settings(message: dict[str, Any]) -> CameraSettings:
    return CameraSettings(
        view=View(
            position=Vector3(*get(message, "position", list[float])),
            direction=Vector3(*get(message, "direction", list[float])),
            up=Vector3(*get(message, "up", list[float])),
        ),
        near_clip=get(message, "near_clip", float),
    )


async def create_camera(
    connection: Connection, typename: str, settings: CameraSettings, derived: dict[str, Any]
) -> CameraId:
    base = serialize_camera_settings(settings)
    id = await create_composed_object(connection, typename, base, derived)
    return CameraId(id)


async def get_camera_settings(connection: Connection, id: CameraId) -> CameraSettings:
    result = await get_specific_object(connection, "camera", id)
    return deserialize_camera_settings(result)


async def update_camera_settings(connection: Connection, id: CameraId, settings: CameraSettings) -> None:
    properties = serialize_camera_settings(settings)
    await update_specific_object(connection, "camera", id, properties)


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

    @property
    def near_clip(self) -> float:
        return self._settings.near_clip

    @near_clip.setter
    def near_clip(self, value: float) -> None:
        self._settings.near_clip = value

    async def push(self, connection: Connection) -> None:
        await update_camera_settings(connection, self._id, self._settings)
        await self._protocol.push(connection, self._id)

    async def pull(self, connection: Connection) -> None:
        self._settings = await get_camera_settings(connection, self._id)
        await self._protocol.pull(connection, self._id)

    def look_at(self, target: Box3) -> None:
        distance = self._protocol.look_at(target)
        distance = max(distance, self.near_clip)
        distance += target.depth / 2
        self.view = View(position=distance * Z)


@dataclass
class PerspectiveSettings:
    fovy: float = math.radians(45)


def serialize_perspective_settings(value: PerspectiveSettings) -> dict[str, Any]:
    return {"fovy": math.degrees(value.fovy)}


def deserialize_perspective_settings(message: dict[str, Any]) -> PerspectiveSettings:
    return PerspectiveSettings(fovy=math.radians(get(message, "fovy", float)))


async def get_perspective_settings(connection: Connection, id: CameraId) -> PerspectiveSettings:
    result = await get_specific_object(connection, "perspective-camera", id)
    return deserialize_perspective_settings(result)


async def update_perspective_settings(connection: Connection, id: CameraId, settings: PerspectiveSettings) -> None:
    properties = serialize_perspective_settings(settings)
    await update_specific_object(connection, "perspective-camera", id, properties)


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
    id = await create_camera(connection, "perspective-camera", settings, derived)
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
    result = await get_specific_object(connection, "orthographic-camera", id)
    return deserialize_orthographic_settings(result)


async def update_orthographic_settings(connection: Connection, id: CameraId, settings: OrthographicSettings) -> None:
    properties = serialize_orthographic_settings(settings)
    await update_specific_object(connection, "orthographic-camera", id, properties)


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
    id = await create_camera(connection, "orthographic-camera", settings, derived)
    return OrthographicCamera(id, replace(settings), replace(orthographic))


async def get_orthographic_camera(connection: Connection, id: CameraId) -> OrthographicCamera:
    return OrthographicCamera(
        id=id,
        settings=await get_camera_settings(connection, id),
        orthographic=await get_orthographic_settings(connection, id),
    )
