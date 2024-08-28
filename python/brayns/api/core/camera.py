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
from dataclasses import dataclass, field
from enum import Enum
from typing import Any, NewType

from brayns.network.connection import Connection
from brayns.utils.box import Box2
from brayns.utils.parsing import deserialize_box, deserialize_vector, get, serialize_box, try_get
from brayns.utils.vector import Vector2, Vector3
from brayns.utils.view import View

from .objects import create_composed_object, get_specific_object, update_specific_object

CameraId = NewType("CameraId", int)


@dataclass
class CameraSettings:
    view: View = field(default_factory=View)
    near_clip: float = 0.0
    image_region: Box2 = Box2(Vector2(0, 0), Vector2(1, 1))


def serialize_camera_settings(settings: CameraSettings) -> dict[str, Any]:
    return {
        "view": {
            "position": settings.view.position,
            "direction": settings.view.direction,
            "up": settings.view.up,
        },
        "nearClip": settings.near_clip,
        "imageRegion": serialize_box(settings.image_region),
    }


def deserialize_camera_settings(message: dict[str, Any]) -> CameraSettings:
    view = get(message, "view", dict[str, Any])

    return CameraSettings(
        view=View(
            position=deserialize_vector(view, "position", Vector3),
            direction=deserialize_vector(view, "direction", Vector3),
            up=deserialize_vector(view, "up", Vector3),
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


async def get_camera(connection: Connection, id: CameraId) -> CameraSettings:
    result = await get_specific_object(connection, "Camera", id)
    return deserialize_camera_settings(result)


async def update_camera(connection: Connection, id: CameraId, settings: CameraSettings) -> None:
    properties = serialize_camera_settings(settings)
    await update_specific_object(connection, "Camera", id, properties)


@dataclass
class DepthOfField:
    aperture_radius: float = 0.0
    focus_distance: float = 1.0


class StereoMode(Enum):
    LEFT = "Left"
    RIGHT = "Right"
    SIDE_BY_SIDE = "SideBySide"
    TOP_BOTTOM = "TopBottom"


@dataclass
class Stereo:
    mode: StereoMode = StereoMode.SIDE_BY_SIDE
    interpupillary_distance: float = 0.0635


def serialize_stereo(stereo: Stereo) -> dict[str, Any]:
    return {
        "mode": stereo.mode.value,
        "interpupillaryDistance": stereo.interpupillary_distance,
    }


def deserialize_stereo(message: dict[str, Any]) -> Stereo:
    return Stereo(
        mode=StereoMode(get(message, "mode", str)),
        interpupillary_distance=get(message, "interpupillaryDistance", float),
    )


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
        stereo = serialize_stereo(settings.stereo)

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
        stereo = deserialize_stereo(stereo)

    return PerspectiveSettings(
        fovy=math.radians(get(message, "fovy", float)),
        depth_of_field=depth_of_field,
        architectural=get(message, "architectural", bool),
        stereo=stereo,
    )


async def create_perspective_camera(
    connection: Connection, settings: CameraSettings, perspective: PerspectiveSettings
) -> CameraId:
    derived = serialize_perspective_settings(perspective)
    return await create_camera(connection, "PerspectiveCamera", settings, derived)


async def get_perspective_camera(connection: Connection, id: CameraId) -> PerspectiveSettings:
    result = await get_specific_object(connection, "PerspectiveCamera", id)
    return deserialize_perspective_settings(result)


async def update_perspective_camera(connection: Connection, id: CameraId, settings: PerspectiveSettings) -> None:
    properties = serialize_perspective_settings(settings)
    await update_specific_object(connection, "PerspectiveCamera", id, properties)


@dataclass
class OrthographicSettings:
    height: float = 1.0


def serialize_orthographic_settings(value: OrthographicSettings) -> dict[str, Any]:
    return {"height": value.height}


def deserialize_orthographic_settings(message: dict[str, Any]) -> OrthographicSettings:
    return OrthographicSettings(height=get(message, "height", float))


async def create_orthographic_camera(
    connection: Connection, settings: CameraSettings, orthographic: OrthographicSettings
) -> CameraId:
    derived = serialize_orthographic_settings(orthographic)
    return await create_camera(connection, "OrthographicCamera", settings, derived)


async def get_orthographic_camera(connection: Connection, id: CameraId) -> OrthographicSettings:
    result = await get_specific_object(connection, "OrthographicCamera", id)
    return deserialize_orthographic_settings(result)


async def update_orthographic_camera(connection: Connection, id: CameraId, settings: OrthographicSettings) -> None:
    properties = serialize_orthographic_settings(settings)
    await update_specific_object(connection, "OrthographicCamera", id, properties)


@dataclass
class PanoramicSettings:
    stereo: Stereo | None = None


def serialize_panoramic_settings(settings: PanoramicSettings) -> dict[str, Any]:
    stereo = None

    if settings.stereo is not None:
        stereo = serialize_stereo(settings.stereo)

    return {"stereo": stereo}


def deserialize_panoramic_settings(message: dict[str, Any]) -> PanoramicSettings:
    stereo = try_get(message, "stereo", dict[str, Any])

    if stereo is not None:
        stereo = deserialize_stereo(stereo)

    return PanoramicSettings(stereo)


async def create_panoramic_camera(
    connection: Connection, settings: CameraSettings, panoramic: PanoramicSettings
) -> CameraId:
    derived = serialize_panoramic_settings(panoramic)
    return await create_camera(connection, "PanoramicCamera", settings, derived)


async def get_panoramic_camera(connection: Connection, id: CameraId) -> PanoramicSettings:
    result = await get_specific_object(connection, "PanoramicCamera", id)
    return deserialize_panoramic_settings(result)


async def update_panoramic_camera(connection: Connection, id: CameraId, settings: PanoramicSettings) -> None:
    properties = serialize_panoramic_settings(settings)
    await update_specific_object(connection, "PanoramicCamera", id, properties)
