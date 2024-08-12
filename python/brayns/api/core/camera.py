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
from dataclasses import dataclass, replace
from typing import Any, NewType, Protocol

from brayns.network.connection import Connection
from brayns.utils.box import Box3
from brayns.utils.parsing import check_type, get
from brayns.utils.vector import Vector3
from brayns.utils.view import FRONT_VIEW, View, Z

CameraId = NewType("CameraId", int)


@dataclass
class CameraSettings:
    view: View = FRONT_VIEW
    near_clipping_distance: float = 0.0


def serialize_camera_settings(value: CameraSettings) -> dict[str, Any]:
    return {
        "position": value.view.position,
        "direction": value.view.direction,
        "up": value.view.up,
        "near_clipping_distance": value.near_clipping_distance,
    }


def deserialize_camera_settings(message: dict[str, Any]) -> CameraSettings:
    return CameraSettings(
        view=View(
            position=Vector3(*get(message, "position", list[float])),
            direction=Vector3(*get(message, "direction", list[float])),
            up=Vector3(*get(message, "up", list[float])),
        ),
        near_clipping_distance=get(message, "near_clipping_distance", float),
    )


async def create_camera(
    connection: Connection, typename: str, base: CameraSettings, derived: dict[str, Any]
) -> CameraId:
    method = "create-" + typename
    params = {
        "base": serialize_camera_settings(base),
        "derived": derived,
    }
    result = await connection.get_result(method, params)

    check_type(result, dict[str, Any])

    return CameraId(get(result, "id", int))


async def get_camera_settings(connection: Connection, id: CameraId) -> CameraSettings:
    result = await connection.get_result("get-camera", {"id": id})
    check_type(result, dict[str, Any])
    return deserialize_camera_settings(result)


async def update_camera_settings(connection: Connection, id: CameraId, settings: CameraSettings) -> None:
    params = {"id": id, "properties": serialize_camera_settings(settings)}
    await connection.get_result("update-camera", params)


@dataclass
class PerspectiveSettings:
    fovy: float = math.radians(45)


def serialize_perspective_settings(value: PerspectiveSettings) -> dict[str, Any]:
    return {"fovy": math.degrees(value.fovy)}


def deserialize_perspective_settings(message: dict[str, Any]) -> PerspectiveSettings:
    return PerspectiveSettings(fovy=math.radians(get(message, "fovy", float)))


async def get_perspective_settings(connection: Connection, id: CameraId) -> PerspectiveSettings:
    result = await connection.get_result("get-perspective-camera", {"id": id})
    check_type(result, dict[str, Any])
    return deserialize_perspective_settings(result)


async def update_perspective_settings(connection: Connection, id: CameraId, settings: PerspectiveSettings) -> None:
    params = serialize_perspective_settings(settings)
    await connection.get_result("update-perspective-camera", {"id": id, "properties": params})


def get_perspective_distance(fovy: float, target_height: float) -> float:
    return target_height / 2 / math.tan(fovy / 2)


@dataclass
class OrthographicSettings:
    height: float = 1.0


def serialize_orthographic_settings(value: OrthographicSettings) -> dict[str, Any]:
    return {"height": value.height}


def deserialize_orthographic_settings(message: dict[str, Any]) -> OrthographicSettings:
    return OrthographicSettings(height=get(message, "height", float))


async def get_orthographic_settings(connection: Connection, id: CameraId) -> OrthographicSettings:
    result = await connection.get_result("get-orthographic-camera", {"id": id})
    check_type(result, dict[str, Any])
    return deserialize_orthographic_settings(result)


async def update_orthographic_settings(connection: Connection, id: CameraId, settings: OrthographicSettings) -> None:
    params = serialize_orthographic_settings(settings)
    await connection.get_result("update-orthographic-camera", {"id": id, "properties": params})


class CameraProtocol(Protocol):
    async def get(self, connection: Connection, id: CameraId) -> None: ...
    async def update(self, connection: Connection, id: CameraId) -> None: ...
    def look_at(self, target: Box3) -> float: ...


class Camera:
    def __init__(
        self,
        id: CameraId,
        settings: CameraSettings,
        protocol: CameraProtocol,
    ) -> None:
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
    def position(self) -> Vector3:
        return self._settings.view.position

    @position.setter
    def position(self, value: Vector3) -> None:
        self._settings.view = replace(self.view, position=value)

    @property
    def direction(self) -> Vector3:
        return self._settings.view.direction

    @direction.setter
    def direction(self, value: Vector3) -> None:
        self._settings.view = replace(self.view, direction=value)

    @property
    def up(self) -> Vector3:
        return self._settings.view.up

    @up.setter
    def up(self, value: Vector3) -> None:
        self._settings.view = replace(self.view, up=value)

    @property
    def near_clipping_distance(self) -> float:
        return self._settings.near_clipping_distance

    @near_clipping_distance.setter
    def near_clipping_distance(self, value: float) -> None:
        self._settings.near_clipping_distance = value

    async def get(self, connection: Connection) -> None:
        self._settings = await get_camera_settings(connection, self._id)
        await self._protocol.get(connection, self._id)

    async def update(self, connection: Connection) -> None:
        await update_camera_settings(connection, self._id, self._settings)
        await self._protocol.update(connection, self._id)

    def look_at(self, target: Box3) -> None:
        distance = self._protocol.look_at(target)
        distance = max(distance, self.near_clipping_distance)
        distance += target.depth / 2
        self.view = replace(FRONT_VIEW, position=distance * Z)


@dataclass
class PerspectiveProtocol(CameraProtocol):
    settings: PerspectiveSettings

    async def get(self, connection: Connection, id: CameraId) -> None:
        self.settings = await get_perspective_settings(connection, id)

    async def update(self, connection: Connection, id: CameraId) -> None:
        await update_perspective_settings(connection, id, self.settings)

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

    @property
    def fovy(self) -> float:
        return self._perspective.settings.fovy

    @fovy.setter
    def fovy(self, value: float) -> None:
        self._perspective.settings.fovy = value


async def create_perspective_camera(
    connection: Connection,
    settings: CameraSettings = CameraSettings(),
    perspective: PerspectiveSettings = PerspectiveSettings(),
) -> PerspectiveCamera:
    typename = "perspective-camera"
    derived = serialize_perspective_settings(perspective)
    id = await create_camera(connection, typename, settings, derived)
    return PerspectiveCamera(id, replace(settings), replace(perspective))


@dataclass
class OrthographicProtocol(CameraProtocol):
    settings: OrthographicSettings

    async def get(self, connection: Connection, id: CameraId) -> None:
        self.settings = await get_orthographic_settings(connection, id)

    async def update(self, connection: Connection, id: CameraId) -> None:
        await update_orthographic_settings(connection, id, self.settings)

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

    @property
    def height(self) -> float:
        return self._orthographic.settings.height

    @height.setter
    def height(self, value: float) -> None:
        self._orthographic.settings.height = value


async def create_orthographic_camera(
    connection: Connection,
    settings: CameraSettings = CameraSettings(),
    orthographic: OrthographicSettings = OrthographicSettings(),
) -> OrthographicCamera:
    typename = "orthographic-camera"
    derived = serialize_orthographic_settings(orthographic)
    id = await create_camera(connection, typename, settings, derived)
    return OrthographicCamera(id, replace(settings), replace(orthographic))
