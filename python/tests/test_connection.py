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

from brayns.network.connection import Response
from brayns.network.json_rpc import (
    JsonRpcError,
    JsonRpcErrorResponse,
    JsonRpcRequest,
    JsonRpcSuccessResponse,
    compose_request,
)
from tests.mock_websocket import mock_connection


def mock_request() -> JsonRpcRequest:
    return JsonRpcRequest("test", 456, b"456", 0)


def mock_response() -> JsonRpcSuccessResponse:
    return JsonRpcSuccessResponse(0, 123, b"123")


def mock_error() -> JsonRpcErrorResponse:
    return JsonRpcErrorResponse(0, JsonRpcError(0, "test"))


@pytest.mark.asyncio
async def test_request() -> None:
    response = mock_response()
    connection, websocket = mock_connection([response])

    request = mock_request()

    result, binary = await connection.request(request.method, request.params, request.binary)

    assert websocket.sent_packets == [compose_request(request)]

    assert result == response.result
    assert binary == response.binary


@pytest.mark.asyncio
async def test_json_rpc() -> None:
    response = mock_response()
    connection, _ = mock_connection([response])

    request = mock_request()

    future = await connection.send_json_rpc(request)

    assert not future.done

    await future.poll()

    assert future.done
    assert await future.wait() == Response(response.result, response.binary)


@pytest.mark.asyncio
async def test_error() -> None:
    jsonrpc = mock_error()
    connection, _ = mock_connection([jsonrpc])

    request = mock_request()

    future = await connection.send_json_rpc(request)

    with pytest.raises(JsonRpcError) as e:
        await future.wait()

    assert e.value == jsonrpc.error
