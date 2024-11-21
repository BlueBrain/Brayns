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
    TextureFilter,
    TextureFormat,
    TextureWrap,
    VoxelType,
    create_linear_transfer_function,
    create_regular_volume,
    create_texture2d,
    create_volume_texture,
    get_texture2d,
    get_volume_texture,
    update_texture2d,
    update_volume_texture,
)


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_texture2d(connection: Connection) -> None:
    data = 4 * b"\1"

    texture = await create_texture2d(
        connection,
        data,
        format=TextureFormat.L8,
        size=(2, 2),
        filter=TextureFilter.NEAREST,
        wrap=TextureWrap.CLAMP_TO_EDGE,
    )

    settings = await get_texture2d(connection, texture)

    assert settings.format == TextureFormat.L8
    assert settings.size == (2, 2)
    assert settings.filter == TextureFilter.NEAREST
    assert settings.wrap == TextureWrap.CLAMP_TO_EDGE

    await update_texture2d(connection, texture, filter=TextureFilter.LINEAR)

    settings = await get_texture2d(connection, texture)
    assert settings.filter == TextureFilter.LINEAR


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_volume_texture(connection: Connection) -> None:
    data = 8 * b"\1"

    volume = await create_regular_volume(connection, data, voxel_type=VoxelType.U8, size=(2, 2, 2))

    transfer_function = await create_linear_transfer_function(
        connection, scalar_range=(0, 1), colors=[(0, 0, 0, 1), (1, 1, 1, 1)]
    )

    texture = await create_volume_texture(connection, volume=volume, transfer_function=transfer_function)

    settings = await get_volume_texture(connection, texture)

    assert settings.volume == volume
    assert settings.transfer_function == transfer_function

    volume = await create_regular_volume(connection, b"\1", voxel_type=VoxelType.U8, size=(1, 1, 1))

    transfer_function = await create_linear_transfer_function(
        connection, scalar_range=(0, 1), colors=[(1, 0, 0, 1), (0, 1, 0, 1)]
    )

    await update_volume_texture(connection, texture, volume=volume, transfer_function=transfer_function)

    settings = await get_volume_texture(connection, texture)

    assert settings.volume == volume
    assert settings.transfer_function == transfer_function
