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
    FramebufferSettings,
    create_framebuffer,
    create_tone_mapper,
    get_framebuffer,
    get_framebuffer_settings,
    remove_objects,
)


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_framebuffer(connection: Connection) -> None:
    settings = FramebufferSettings(resolution=(1920, 1080))
    framebuffer = await create_framebuffer(connection, settings)

    assert framebuffer.id == 1
    assert framebuffer.settings == settings

    retreived = await get_framebuffer(connection, framebuffer.id)

    assert retreived.id == framebuffer.id
    assert retreived.settings == framebuffer.settings

    settings2 = await get_framebuffer_settings(connection, framebuffer.id)
    assert settings2 == settings


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_remove_operations(connection: Connection) -> None:
    tone_mapper = await create_tone_mapper(connection)

    settings = FramebufferSettings(image_operations={tone_mapper.id})
    framebuffer = await create_framebuffer(connection, settings)

    before_remove = await get_framebuffer_settings(connection, framebuffer.id)
    assert before_remove.image_operations == {tone_mapper.id}

    await remove_objects(connection, [tone_mapper.id])

    after_remove = await get_framebuffer_settings(connection, framebuffer.id)
    assert after_remove.image_operations == {0}
