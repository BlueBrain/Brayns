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
    create_ao_material,
    create_ao_renderer,
    create_path_tracer,
    create_scivis_renderer,
    get_ao_renderer,
    get_path_tracer,
    get_renderer,
    get_scivis_renderer,
    remove_objects,
    update_ao_renderer,
    update_path_tracer,
    update_renderer,
    update_scivis_renderer,
)


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_renderer(connection: Connection) -> None:
    renderer = await create_ao_renderer(connection, samples=2)

    settings = await get_renderer(connection, renderer)
    assert settings.samples == 2

    await update_renderer(connection, renderer, samples=3)

    settings = await get_renderer(connection, renderer)
    assert settings.samples == 3


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_ao_renderer(connection: Connection) -> None:
    renderer = await create_ao_renderer(
        connection,
        ao_samples=2,
        ao_distance=10,
        ao_intensity=2,
        volume_sampling_rate=2,
    )

    settings = await get_ao_renderer(connection, renderer)
    assert settings.ao_samples == 2
    assert settings.ao_distance == 10
    assert settings.ao_intensity == 2
    assert settings.volume_sampling_rate == 2

    await update_ao_renderer(connection, renderer, ao_distance=5)
    settings.ao_distance = 5

    assert await get_ao_renderer(connection, renderer) == settings


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_scivis_renderer(connection: Connection) -> None:
    renderer = await create_scivis_renderer(
        connection,
        shadows=True,
        ao_samples=2,
        ao_distance=10,
        volume_sampling_rate=2,
        show_visible_lights=True,
    )

    settings = await get_scivis_renderer(connection, renderer)
    assert settings.shadows
    assert settings.ao_samples == 2
    assert settings.ao_distance == 10
    assert settings.volume_sampling_rate == 2
    assert settings.show_visible_lights

    await update_scivis_renderer(connection, renderer, ao_distance=5)
    settings.ao_distance = 5

    assert await get_scivis_renderer(connection, renderer) == settings


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_path_tracer(connection: Connection) -> None:
    renderer = await create_path_tracer(connection)

    settings = await get_path_tracer(connection, renderer)

    await update_path_tracer(connection, renderer)

    assert await get_path_tracer(connection, renderer) == settings


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_materials(connection: Connection) -> None:
    material = await create_ao_material(connection)
    renderer = await create_ao_renderer(connection, materials=[material])

    settings = await get_renderer(connection, renderer)
    assert settings.materials == [material]

    await remove_objects(connection, [material])

    settings = await get_renderer(connection, renderer)
    assert settings.materials[0].id == 0
