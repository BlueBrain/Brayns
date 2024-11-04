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
    VolumeFilter,
    VoxelType,
    create_regular_volume,
    get_regular_volume,
    update_regular_volume,
)


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_regular_volume(connection: Connection) -> None:
    data = 8 * b"\1"

    volume = await create_regular_volume(connection, data, voxel_type=VoxelType.U8, size=(2, 2, 2))

    settings = await get_regular_volume(connection, volume)

    settings.filter = VolumeFilter.CUBIC
    await update_regular_volume(connection, volume, filter=settings.filter)

    assert settings == await get_regular_volume(connection, volume)
