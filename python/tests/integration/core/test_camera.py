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


import pytest

from brayns import (
    CameraSettings,
    Connection,
    JsonRpcError,
    OrthographicCamera,
    OrthographicSettings,
    PanoramicSettings,
    PerspectiveSettings,
    Stereo,
    StereoMode,
    clear_objects,
    create_orthographic_camera,
    create_panoramic_camera,
    create_perspective_camera,
    get_camera,
    get_object,
    get_orthographic_camera,
    get_panoramic_camera,
    get_perspective_camera,
    remove_objects,
    update_camera,
    update_object,
    update_orthographic_camera,
    update_panoramic_camera,
    update_perspective_camera,
)


def check_perspective(left: PerspectiveSettings, right: PerspectiveSettings) -> None:
    assert left.architectural == right.architectural
    assert left.depth_of_field == right.depth_of_field
    assert left.stereo == right.stereo
    assert left.fovy == pytest.approx(right.fovy)


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_perspective_camera(connection: Connection) -> None:
    settings = CameraSettings()
    perspective = PerspectiveSettings(fovy=1.0)
    camera = await create_perspective_camera(connection, settings, perspective)

    assert settings == await get_camera(connection, camera)

    check_perspective(perspective, await get_perspective_camera(connection, camera))

    settings.near_clip = 3
    await update_camera(connection, camera, settings)

    perspective.stereo = Stereo(mode=StereoMode.SIDE_BY_SIDE)
    await update_perspective_camera(connection, camera, perspective)

    assert settings == await get_camera(connection, camera)

    check_perspective(perspective, await get_perspective_camera(connection, camera))


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_orthographic_camera(connection: Connection) -> None:
    settings = CameraSettings()
    orthographic = OrthographicSettings(height=2.0)
    camera = await create_orthographic_camera(connection, settings, orthographic)

    assert settings == await get_camera(connection, camera)
    assert orthographic == await get_orthographic_camera(connection, camera)

    settings.near_clip = 3
    await update_camera(connection, camera, settings)

    orthographic.height = 3.0
    await update_orthographic_camera(connection, camera, orthographic)

    assert settings == await get_camera(connection, camera)
    assert orthographic == await get_orthographic_camera(connection, camera)


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_panoramic_camera(connection: Connection) -> None:
    settings = CameraSettings()
    panoramic = PanoramicSettings()
    camera = await create_panoramic_camera(connection, settings, panoramic)

    assert settings == await get_camera(connection, camera)
    assert panoramic == await get_panoramic_camera(connection, camera)

    settings.near_clip = 3
    await update_camera(connection, camera, settings)

    panoramic.stereo = Stereo(mode=StereoMode.SIDE_BY_SIDE)
    await update_panoramic_camera(connection, camera, panoramic)

    assert settings == await get_camera(connection, camera)
    assert panoramic == await get_panoramic_camera(connection, camera)


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_object_management(connection: Connection) -> None:
    settings = CameraSettings()
    perspective = PerspectiveSettings()
    orthographic = OrthographicSettings()

    camera1 = await create_perspective_camera(connection, settings, perspective)
    camera2 = await create_orthographic_camera(connection, settings, orthographic)

    with pytest.raises(JsonRpcError):
        await get_orthographic_camera(connection, OrthographicCamera(camera1.id))

    await update_object(connection, camera1, "test")

    object1 = await get_object(connection, camera1)

    assert object1.id == camera1.id
    assert object1.type == "PerspectiveCamera"
    assert object1.user_data == "test"

    object2 = await get_object(connection, camera2)

    assert object2.id == camera2.id
    assert object2.type == "OrthographicCamera"
    assert object2.user_data is None

    await remove_objects(connection, [camera1])

    with pytest.raises(JsonRpcError):
        await get_object(connection, camera1)

    await get_object(connection, camera2)

    await clear_objects(connection)

    with pytest.raises(JsonRpcError):
        await get_object(connection, camera2)
