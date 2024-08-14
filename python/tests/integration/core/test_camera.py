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

import pytest

from brayns import (
    Camera,
    Connection,
    JsonRpcError,
    Vector3,
    Y,
    Z,
    clear_objects,
    create_orthographic_camera,
    create_perspective_camera,
    get_camera_settings,
    get_object,
    get_orthographic_camera,
    get_orthographic_settings,
    get_perspective_camera,
    get_perspective_settings,
    remove_objects,
    update_camera_settings,
    update_object,
    update_orthographic_settings,
    update_perspective_settings,
)


def check_camera_defaults(camera: Camera) -> None:
    assert camera.id == 1
    assert camera.view.position == Vector3()
    assert camera.view.direction == -Z
    assert camera.view.up == Y
    assert camera.near_clip == 0


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_perspective_camera(connection: Connection) -> None:
    camera = await create_perspective_camera(connection)

    check_camera_defaults(camera)
    assert camera.perspective.fovy == pytest.approx(math.radians(45))

    await camera.pull(connection)
    check_camera_defaults(camera)
    assert camera.perspective.fovy == pytest.approx(math.radians(45))

    camera.view.position = Vector3(1, 2, 3)
    camera.perspective.fovy = math.radians(60)
    await camera.push(connection)

    settings = await get_camera_settings(connection, camera.id)
    assert camera.settings == settings

    perspective = await get_perspective_settings(connection, camera.id)
    assert camera.perspective.fovy == pytest.approx(perspective.fovy)

    settings.near_clip = 10
    await update_camera_settings(connection, camera.id, settings)

    perspective.fovy = math.radians(30)
    await update_perspective_settings(connection, camera.id, perspective)

    await camera.pull(connection)
    assert camera.settings == settings
    assert camera.perspective.fovy == pytest.approx(perspective.fovy)

    retreived = await get_perspective_camera(connection, camera.id)
    assert retreived.id == camera.id
    assert retreived.settings == camera.settings
    assert retreived.perspective == camera.perspective


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_orthographic_camera(connection: Connection) -> None:
    camera = await create_orthographic_camera(connection)

    check_camera_defaults(camera)
    assert camera.orthographic.height == 1

    await camera.pull(connection)

    check_camera_defaults(camera)
    assert camera.orthographic.height == 1

    camera.view.position = Vector3(1, 2, 3)
    camera.orthographic.height = 2
    await camera.push(connection)

    settings = await get_camera_settings(connection, camera.id)
    assert camera.settings == settings

    orthographic = await get_orthographic_settings(connection, camera.id)
    assert camera.orthographic == orthographic

    settings.near_clip = 10
    await update_camera_settings(connection, camera.id, settings)

    orthographic.height = 3
    await update_orthographic_settings(connection, camera.id, orthographic)

    await camera.pull(connection)
    assert camera.settings == settings
    assert camera.orthographic == orthographic

    retreived = await get_orthographic_camera(connection, camera.id)
    assert retreived.id == camera.id
    assert retreived.settings == camera.settings
    assert retreived.orthographic == camera.orthographic


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_object_management(connection: Connection) -> None:
    camera1 = await create_perspective_camera(connection)
    camera2 = await create_orthographic_camera(connection)
    camera3 = await create_orthographic_camera(connection)

    await update_object(connection, camera1.id, "test")

    object1 = await get_object(connection, camera1.id)

    assert object1.id == camera1.id
    assert object1.type == "perspective-camera"
    assert object1.user_data == "test"

    object2 = await get_object(connection, camera2.id)

    assert object2.id == camera2.id
    assert object2.type == "orthographic-camera"
    assert object2.user_data is None

    await remove_objects(connection, [camera1.id, camera2.id])

    with pytest.raises(JsonRpcError):
        await get_object(connection, camera1.id)

    with pytest.raises(JsonRpcError):
        await get_object(connection, camera2.id)

    await get_object(connection, camera3.id)

    await clear_objects(connection)

    with pytest.raises(JsonRpcError):
        await get_object(connection, camera3.id)
