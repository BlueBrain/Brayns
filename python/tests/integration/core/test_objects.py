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

from brayns import (
    get_all_objects,
    get_object,
    clear_objects,
    remove_objects,
    Connection,
    JsonRpcError,
    create_object,
)

import pytest


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_create_object(connection: Connection) -> None:
    obj = await create_object(connection, "test")
    assert obj.id == 1
    assert obj.metadata.type == "object"
    assert obj.user_data == "test"


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_get_all_objects(connection: Connection) -> None:
    objects = [await create_object(connection) for _ in range(10)]
    tests = await get_all_objects(connection)
    assert tests == objects


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_get_object(connection: Connection) -> None:
    objects = [await create_object(connection) for _ in range(10)]
    tests = [await get_object(connection, obj.id) for obj in objects]
    assert tests == objects

    with pytest.raises(JsonRpcError):
        await get_object(connection, 123)


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_remove_objects(connection: Connection) -> None:
    objects = [await create_object(connection) for _ in range(10)]
    await remove_objects(connection, [1, 2, 3])
    tests = await get_all_objects(connection)
    assert [obj.id for obj in tests] == list(range(4, 11))

    for obj in objects[:3]:
        with pytest.raises(JsonRpcError):
            await get_object(connection, obj.id)


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_clear_objects(connection: Connection) -> None:
    objects = [await create_object(connection) for _ in range(10)]
    await clear_objects(connection)
    assert not await get_all_objects(connection)

    for obj in objects:
        with pytest.raises(JsonRpcError):
            await get_object(connection, obj.id)
