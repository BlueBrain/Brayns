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
    FramebufferSettings,
    JpegChannel,
    JsonRpcError,
    PngChannel,
    create_framebuffer,
    read_framebuffer,
    read_framebuffer_as_exr,
    read_framebuffer_as_jpeg,
    read_framebuffer_as_png,
)


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_read_invalid_channel(connection: Connection) -> None:
    settings = FramebufferSettings()
    framebuffer = await create_framebuffer(connection, settings)

    channel = FramebufferChannel.PRIMITIVE_ID

    with pytest.raises(JsonRpcError):
        await read_framebuffer(connection, framebuffer, channel)


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_read_framebuffer(connection: Connection) -> None:
    settings = FramebufferSettings(channels={FramebufferChannel.PRIMITIVE_ID})
    framebuffer = await create_framebuffer(connection, settings)

    channel = FramebufferChannel.PRIMITIVE_ID
    data = await read_framebuffer(connection, framebuffer, channel)

    primitive_id = int.from_bytes(data[:4], "little", signed=False)

    assert primitive_id == 0


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_read_framebuffer_as_jpeg(connection: Connection) -> None:
    settings = FramebufferSettings()
    framebuffer = await create_framebuffer(connection, settings)

    channel = JpegChannel.COLOR
    data = await read_framebuffer_as_jpeg(connection, framebuffer, channel)

    assert data


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_read_framebuffer_as_png(connection: Connection) -> None:
    settings = FramebufferSettings()
    framebuffer = await create_framebuffer(connection, settings)

    channel = PngChannel.COLOR
    data = await read_framebuffer_as_png(connection, framebuffer, channel)

    assert data


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_read_framebuffer_as_exr(connection: Connection) -> None:
    channels = set(FramebufferChannel)
    settings = FramebufferSettings(format=FramebufferFormat.RGBA8, channels=channels)
    framebuffer = await create_framebuffer(connection, settings)

    data = await read_framebuffer_as_exr(connection, framebuffer, channels)

    assert data
