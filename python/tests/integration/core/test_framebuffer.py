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
    create_framebuffer,
    create_tone_mapper,
    get_framebuffer,
    remove_objects,
)


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_framebuffer(connection: Connection) -> None:
    tone_mapper = await create_tone_mapper(connection)

    framebuffer = await create_framebuffer(
        connection,
        resolution=(1920, 1080),
        format=FramebufferFormat.RGBA32F,
        channels=set(FramebufferChannel),
        accumulation=True,
        variance=True,
        operations=[tone_mapper],
    )

    settings = await get_framebuffer(connection, framebuffer)

    assert settings.resolution == (1920, 1080)
    assert settings.format == FramebufferFormat.RGBA32F
    assert settings.channels == set(FramebufferChannel)
    assert settings.accumulation
    assert settings.variance
    assert settings.operations == [tone_mapper]
    assert settings.variance_estimate is None


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_remove_operations(connection: Connection) -> None:
    tone_mapper = await create_tone_mapper(connection)
    framebuffer = await create_framebuffer(connection, operations=[tone_mapper])

    before_remove = await get_framebuffer(connection, framebuffer)
    assert before_remove.operations == [tone_mapper]

    await remove_objects(connection, [tone_mapper])

    after_remove = await get_framebuffer(connection, framebuffer)
    assert after_remove.operations[0].id == 0
