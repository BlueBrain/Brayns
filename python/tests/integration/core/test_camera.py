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
    Connection,
    Stereo,
    create_orthographic_camera,
    create_panoramic_camera,
    create_perspective_camera,
    get_camera,
    get_orthographic_camera,
    get_panoramic_camera,
    get_perspective_camera,
    update_camera,
    update_orthographic_camera,
    update_panoramic_camera,
    update_perspective_camera,
)


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_camera(connection: Connection) -> None:
    camera = await create_perspective_camera(connection)

    settings = await get_camera(connection, camera)

    await update_camera(connection, camera, position=(1, 2, 3))

    settings.position = (1, 2, 3)

    assert settings == await get_camera(connection, camera)


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_perspective_camera(connection: Connection) -> None:
    camera = await create_perspective_camera(connection)

    settings = await get_perspective_camera(connection, camera)

    await update_perspective_camera(connection, camera, fovy=1.0)

    settings.fovy = 1.0

    assert settings == await get_perspective_camera(connection, camera)


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_orthographic_camera(connection: Connection) -> None:
    camera = await create_orthographic_camera(connection)

    settings = await get_orthographic_camera(connection, camera)

    await update_orthographic_camera(connection, camera, height=1.0)

    settings.height = 1.0

    assert settings == await get_orthographic_camera(connection, camera)


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_panoramic_camera(connection: Connection) -> None:
    camera = await create_panoramic_camera(connection)

    settings = await get_panoramic_camera(connection, camera)

    await update_panoramic_camera(connection, camera, stereo=Stereo.RIGHT)

    settings.stereo = Stereo.RIGHT

    assert settings == await get_panoramic_camera(connection, camera)
