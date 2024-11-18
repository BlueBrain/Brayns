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

from brayns import Connection, JsonRpcError, create_quad_mesh, create_triangle_mesh, get_quad_mesh, get_triangle_mesh


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_triangle_mesh(connection: Connection) -> None:
    minimal = await create_triangle_mesh(connection, positions=[(0, 0, 0), (1, 1, 1), (1, 0, 1)])

    settings = await get_triangle_mesh(connection, minimal)
    assert settings.positions
    assert not settings.normals
    assert not settings.colors
    assert not settings.uvs
    assert not settings.indices

    mesh = await create_triangle_mesh(
        connection,
        positions=[(0, 0, 0), (1, 1, 1), (1, 0, 1)],
        normals=[(0, 0, 0), (1, 1, 1), (1, 0, 1)],
        colors=[(1, 0, 0, 1), (0, 1, 0, 1), (0, 0, 1, 1)],
        uvs=[(1, 0), (0, 1), (1, 1)],
        indices=[(0, 1, 2)],
    )

    settings = await get_triangle_mesh(connection, mesh)
    assert settings.positions
    assert settings.normals
    assert settings.colors
    assert settings.uvs
    assert settings.indices

    with pytest.raises(JsonRpcError):
        await create_triangle_mesh(connection, positions=[])

    with pytest.raises(JsonRpcError):
        await create_triangle_mesh(connection, positions=[(0, 1, 2), (4, 5, 6)], normals=[(0, 1, 2)])


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_quad_mesh(connection: Connection) -> None:
    mesh = await create_quad_mesh(
        connection,
        positions=[(0, 0, 0), (1, 1, 1), (1, 0, 1), (0, 0, 1)],
        normals=[(0, 0, 0), (1, 1, 1), (1, 0, 1), (0, 0, 1)],
        colors=[(1, 0, 0, 1), (0, 1, 0, 1), (0, 0, 1, 1), (0, 0, 1, 1)],
        uvs=[(1, 0), (0, 1), (1, 1), (1, 0)],
        indices=[(0, 1, 2, 3)],
    )

    settings = await get_quad_mesh(connection, mesh)
    assert settings.positions
    assert settings.normals
    assert settings.colors
    assert settings.uvs
    assert settings.indices
