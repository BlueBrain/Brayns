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
    VERSION,
    Connection,
    JsonRpcError,
    cancel_task,
    get_endpoint,
    get_methods,
    get_task,
    get_task_result,
    get_tasks,
    get_version,
)


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_version(connection: Connection) -> None:
    version = await get_version(connection)
    assert version.tag == VERSION


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_methods(connection: Connection) -> None:
    methods = await get_methods(connection)
    assert all(isinstance(method, str) for method in methods)


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_schema(connection: Connection) -> None:
    endpoint = await get_endpoint(connection, "getVersion")
    assert endpoint.method == "getVersion"
    assert isinstance(endpoint.description, str)
    assert isinstance(endpoint.params_schema, dict)
    assert isinstance(endpoint.result_schema, dict)


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_tasks(connection: Connection) -> None:
    tasks = await get_tasks(connection)
    assert not tasks

    with pytest.raises(JsonRpcError):
        await get_task(connection, 0)

    with pytest.raises(JsonRpcError):
        await cancel_task(connection, 0)

    with pytest.raises(JsonRpcError):
        await get_task_result(connection, 0)
