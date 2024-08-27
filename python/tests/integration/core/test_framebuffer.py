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
    Accumulation,
    Connection,
    FramebufferChannel,
    FramebufferFormat,
    FramebufferSettings,
    Resolution,
    ToneMapperSettings,
    create_framebuffer,
    create_tone_mapper,
    get_framebuffer,
    remove_objects,
    update_framebuffer,
)


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_framebuffer(connection: Connection) -> None:
    settings = FramebufferSettings(
        resolution=Resolution(1920, 1080),
        format=FramebufferFormat.RGBA32F,
        channels=set(FramebufferChannel),
        accumulation=Accumulation(variance=True),
    )

    framebuffer = await create_framebuffer(connection, settings)

    info = await get_framebuffer(connection, framebuffer)
    assert info.settings == settings
    assert info.variance is None

    tone_mapper_settings = ToneMapperSettings()
    tone_mapper = await create_tone_mapper(connection, tone_mapper_settings)

    settings.image_operations = [tone_mapper]
    await update_framebuffer(connection, framebuffer, settings.image_operations)

    info = await get_framebuffer(connection, framebuffer)
    assert info.settings == settings
    assert info.variance is None


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_remove_operations(connection: Connection) -> None:
    tone_mapper_settings = ToneMapperSettings()
    tone_mapper = await create_tone_mapper(connection, tone_mapper_settings)

    settings = FramebufferSettings(image_operations=[tone_mapper])
    framebuffer = await create_framebuffer(connection, settings)

    before_remove = await get_framebuffer(connection, framebuffer)
    assert before_remove.settings.image_operations == [tone_mapper]

    await remove_objects(connection, [tone_mapper])

    after_remove = await get_framebuffer(connection, framebuffer)
    assert after_remove.settings.image_operations == [0]
