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
    JsonRpcError,
    Object,
    clear_objects,
    create_empty_object,
    get_all_objects,
    get_object,
    remove_objects,
    update_object,
)


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_create_empty_object(connection: Connection) -> None:
    object = await create_empty_object(connection)
    assert object.id == 1


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_get_all_objects(connection: Connection) -> None:
    created = [await create_empty_object(connection) for _ in range(10)]
    retreived = await get_all_objects(connection)

    assert all(x.id == y.id for x, y in zip(created, retreived))


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_get_object(connection: Connection) -> None:
    created = [await create_empty_object(connection) for _ in range(10)]
    retreived = await get_all_objects(connection)
    tests = [await get_object(connection, object) for object in created]

    assert all(x.id == y.id for x, y in zip(tests, retreived))

    with pytest.raises(JsonRpcError):
        await get_object(connection, Object(123))


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_update_object(connection: Connection) -> None:
    objects = [await create_empty_object(connection) for _ in range(10)]

    await update_object(connection, objects[0], "test")

    test = await get_object(connection, objects[0])

    assert test.id == 1
    assert test.type == "EmptyObject"
    assert test.user_data == "test"


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_remove_objects(connection: Connection) -> None:
    objects = [await create_empty_object(connection) for _ in range(10)]

    await remove_objects(connection, objects[:3])
    tests = await get_all_objects(connection)

    assert [object.id for object in tests] == list(range(4, 11))

    for object in objects[:3]:
        with pytest.raises(JsonRpcError):
            await get_object(connection, object)


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_clear_objects(connection: Connection) -> None:
    objects = [await create_empty_object(connection) for _ in range(10)]

    await clear_objects(connection)

    assert not await get_all_objects(connection)

    for object in objects:
        with pytest.raises(JsonRpcError):
            await get_object(connection, object)
