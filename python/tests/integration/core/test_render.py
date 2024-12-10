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
    FramebufferChannel,
    FramebufferFormat,
    Irradiance,
    JpegChannel,
    create_ambient_light,
    create_framebuffer,
    create_geometric_model,
    create_group,
    create_instance,
    create_orthographic_camera,
    create_scivis_material,
    create_scivis_renderer,
    create_spheres,
    create_world,
    get_framebuffer,
    read_framebuffer_as_jpeg,
    render,
)


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_render(connection: Connection) -> None:
    framebuffer = await create_framebuffer(
        connection,
        resolution=(400, 400),
        format=FramebufferFormat.RGBA32F,
        channels={FramebufferChannel.COLOR},
        accumulation=True,
        variance=True,
    )

    material = await create_scivis_material(connection)
    renderer = await create_scivis_renderer(connection, materials=[material])

    camera = await create_orthographic_camera(
        connection,
        position=(0, 0, 1),
        direction=(0, 0, -1),
        up=(0, 1, 0),
        height=4,
        aspect=1,
    )

    spheres = await create_spheres(
        connection,
        positions_radii=[
            (0, 0, 0, 0.5),
            (1, 0, 0, 0.5),
            (0, 1, 0, 0.5),
        ],
    )

    model = await create_geometric_model(
        connection,
        geometry=spheres,
        materials=[0],
        colors=[
            (1, 0, 0, 1),
            (0, 1, 0, 1),
            (0, 0, 1, 1),
        ],
    )

    light = await create_ambient_light(connection, intensity=Irradiance(10))

    group = await create_group(connection, geometries=[model], lights=[light])
    instance = await create_instance(connection, group=group)
    world = await create_world(connection, instances=[instance])

    result = await render(connection, framebuffer=framebuffer, renderer=renderer, camera=camera, world=world)
    assert result.duration > 0

    settings = await get_framebuffer(connection, framebuffer)
    assert settings.variance is not None

    image = await read_framebuffer_as_jpeg(connection, framebuffer, JpegChannel.COLOR)
    assert image

    # with open("/home/acfleury/source/repos/Brayns/python/test.jpeg", "wb") as file:
    #     file.write(image)
