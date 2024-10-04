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

import json

from brayns.network.json_rpc import (
    JsonRpcError,
    JsonRpcErrorResponse,
    JsonRpcRequest,
    JsonRpcSuccessResponse,
    compose_request,
    parse_response,
)


def test_compose_request() -> None:
    request = JsonRpcRequest("test", 123, id=0)

    data = compose_request(request)

    text = """{"jsonrpc":"2.0","method":"test","params":123,"id":0}"""
    assert data == text

    request.binary = b"123"

    data = compose_request(request)

    assert isinstance(data, bytes)
    assert len(data) == 4 + len(text) + len(request.binary)

    size = int.from_bytes(data[:4], byteorder="little", signed=False)

    assert size == len(text)
    assert data[4 : size + 4].decode() == text
    assert data[size + 4 :] == request.binary


def test_no_id() -> None:
    request = JsonRpcRequest("test")

    data = compose_request(request)

    assert isinstance(data, str)

    message = json.loads(data)

    assert isinstance(message, dict)

    assert "id" not in message


def test_parse_response() -> None:
    text = """{"jsonrpc":"2.0","id":0,"result":123}"""

    assert parse_response(text) == JsonRpcSuccessResponse(0, 123)

    binary = b"123"
    header = len(text).to_bytes(4, byteorder="little", signed=False)
    binary = header + text.encode() + binary

    assert parse_response(binary) == JsonRpcSuccessResponse(0, 123, b"123")


def test_parse_error() -> None:
    text = """{"jsonrpc":"2.0","id":0,"error":{"code":0,"message": "test"}}"""

    assert parse_response(text) == JsonRpcErrorResponse(0, JsonRpcError(0, "test"))
