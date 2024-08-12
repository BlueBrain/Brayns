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
from abc import ABC, abstractmethod
from dataclasses import dataclass, replace
from typing import Any, NewType

from brayns.network.connection import Connection
from brayns.utils.box import Box3
from brayns.utils.parsing import check_type, get
from brayns.utils.vector import Vector3
from brayns.utils.view import View

CameraId = NewType("CameraId", int)


@dataclass
class CameraSettings:
    view: View
    near_clipping_distance: float


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
    return {"fovy": value.fovy}


def deserialize_perspective_settings(message: dict[str, Any]) -> PerspectiveSettings:
    return PerspectiveSettings(fovy=math.radians(get(message, "fovy", float)))


@dataclass
class OrthographicSettings:
    height: float = 1


def serialize_orthographic_settings(value: OrthographicSettings) -> dict[str, Any]:
    return {"height": value.height}


def deserialize_orthographic_settings(message: dict[str, Any]) -> OrthographicSettings:
    return OrthographicSettings(height=get(message, "height", float))


class Camera(ABC):
    def __init__(self, settings: CameraSettings) -> None:
        self.__settings = settings

    @property
    @abstractmethod
    def name(self) -> str: ...

    @property
    def settings(self) -> CameraSettings:
        return self.__settings

    @settings.setter
    def settings(self, value: CameraSettings) -> None:
        self.__settings = value

    @property
    def view(self) -> View:
        return self.__settings.view

    @view.setter
    def view(self, value: View) -> None:
        self.__settings.view = value

    @property
    def position(self) -> Vector3:
        return self.__settings.view.position

    @position.setter
    def position(self, value: Vector3) -> None:
        self.__settings.view = replace(self.view, position=value)

    @property
    def direction(self) -> Vector3:
        return self.__settings.view.direction

    @direction.setter
    def direction(self, value: Vector3) -> None:
        self.__settings.view = replace(self.view, direction=value)

    @property
    def up(self) -> Vector3:
        return self.__settings.view.up

    @up.setter
    def up(self, value: Vector3) -> None:
        self.__settings.view = replace(self.view, up=value)

    @property
    def near_clipping_distance(self) -> float:
        return self.__settings.near_clipping_distance

    @near_clipping_distance.setter
    def near_clipping_distance(self, value: float) -> None:
        self.__settings.near_clipping_distance = value

    @abstractmethod
    def look_at(self, target: Box3) -> None: ...


"""class PerspectiveCamera(Camera[PerspectiveSettings]):
    def __init__(self, base: CameraSettings, derived: PerspectiveSettings) -> None:
        super().__init__(
            "perspective-camera",
            base,
            derived,
            lambda x: serialize_perspective_settings(x),
            lambda x: deserialize_perspective_settings(x),
        )

    @property
    def fovy(self) -> float:
        return self.derived_settings.fovy

    @fovy.setter
    def fovy(self, value: float) -> None:
        self.derived_settings.fovy = value
"""
