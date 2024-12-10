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
    Intensity,
    Irradiance,
    Power,
    Radiance,
    TextureFormat,
    create_ambient_light,
    create_cylinder_light,
    create_distant_light,
    create_hdri_light,
    create_quad_light,
    create_sphere_light,
    create_spot_light,
    create_sunsky_light,
    create_texture2d,
    get_ambient_light,
    get_cylinder_light,
    get_distant_light,
    get_hdri_light,
    get_light,
    get_quad_light,
    get_sphere_light,
    get_spot_light,
    get_sunsky_light,
    remove_objects,
    update_ambient_light,
    update_cylinder_light,
    update_distant_light,
    update_hdri_light,
    update_light,
    update_quad_light,
    update_sphere_light,
    update_spot_light,
    update_sunsky_light,
)


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_light(connection: Connection) -> None:
    light = await create_distant_light(connection)

    settings = await get_light(connection, light)
    assert settings.color == (1, 1, 1)
    assert settings.visible

    await update_light(connection, light, color=(1, 0, 0), visible=False)

    settings = await get_light(connection, light)
    assert settings.color == (1, 0, 0)
    assert not settings.visible


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_distant_light(connection: Connection) -> None:
    light = await create_distant_light(connection, intensity=Radiance(2))

    settings = await get_distant_light(connection, light)
    assert settings.intensity == Radiance(2)

    await update_distant_light(connection, light, direction=(1, 2, 3))

    settings = await get_distant_light(connection, light)
    assert settings.direction == (1, 2, 3)


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_sphere_light(connection: Connection) -> None:
    light = await create_sphere_light(connection, intensity=Intensity(2))

    settings = await get_sphere_light(connection, light)
    assert settings.intensity == Intensity(2)

    await update_sphere_light(connection, light, position=(1, 2, 3))

    settings = await get_sphere_light(connection, light)
    assert settings.position == (1, 2, 3)


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_spot_light(connection: Connection) -> None:
    light = await create_spot_light(connection, intensity=Intensity(2))

    settings = await get_spot_light(connection, light)
    assert settings.intensity == Intensity(2)

    await update_spot_light(connection, light, position=(1, 2, 3))

    settings = await get_spot_light(connection, light)
    assert settings.position == (1, 2, 3)


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_quad_light(connection: Connection) -> None:
    light = await create_quad_light(connection, intensity=Power(2))

    settings = await get_quad_light(connection, light)
    assert settings.intensity == Power(2)

    await update_quad_light(connection, light, position=(1, 2, 3))

    settings = await get_quad_light(connection, light)
    assert settings.position == (1, 2, 3)


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_cylinder_light(connection: Connection) -> None:
    light = await create_cylinder_light(connection, intensity=Power(2))

    settings = await get_cylinder_light(connection, light)
    assert settings.intensity == Power(2)

    await update_cylinder_light(connection, light, start=(1, 2, 3))

    settings = await get_cylinder_light(connection, light)
    assert settings.start == (1, 2, 3)


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_hdri_light(connection: Connection) -> None:
    data = 4 * b"\1"

    texture = await create_texture2d(connection, data, format=TextureFormat.L8, size=(2, 2))

    light = await create_hdri_light(connection, map=texture, intensity_scale=2)

    settings = await get_hdri_light(connection, light)
    assert settings.intensity_scale == 2

    await update_hdri_light(connection, light, up=(1, 2, 3))

    settings = await get_hdri_light(connection, light)
    assert settings.up == (1, 2, 3)

    await remove_objects(connection, [texture])

    settings = await get_hdri_light(connection, light)
    assert settings.map.id == 0


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_ambient_light(connection: Connection) -> None:
    light = await create_ambient_light(connection, intensity=Irradiance(2))

    settings = await get_ambient_light(connection, light)
    assert settings.intensity == Irradiance(2)

    await update_ambient_light(connection, light, intensity=Radiance(3))

    settings = await get_ambient_light(connection, light)
    assert settings.intensity == Radiance(3)


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_sunsky_light(connection: Connection) -> None:
    light = await create_sunsky_light(connection, intensity_scale=2)

    settings = await get_sunsky_light(connection, light)
    assert settings.intensity_scale == 2

    await update_sunsky_light(connection, light, turbidity=5)

    settings = await get_sunsky_light(connection, light)
    assert settings.turbidity == 5
