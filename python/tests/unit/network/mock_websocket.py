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

from dataclasses import dataclass, field

from brayns.network.connection import Connection
from brayns.network.json_rpc import (
    JsonRpcErrorResponse,
    JsonRpcResponse,
    JsonRpcSuccessResponse,
    compose_binary,
    compose_text,
)
from brayns.network.websocket import WebSocket


@dataclass
class MockWebSocket(WebSocket):
    responses: list[bytes | str]
    closed: bool = field(default=False, init=False)
    sent_packets: list[bytes | str] = field(default_factory=list, init=False)

    @property
    def host(self) -> str:
        return "localhost"

    @property
    def port(self) -> int:
        return 5000

    async def close(self) -> None:
        self.closed = True

    async def send(self, data: bytes | str) -> None:
        self.sent_packets.append(data)

    async def receive(self) -> bytes | str:
        return self.responses.pop(0)


def compose_error(response: JsonRpcErrorResponse) -> str:
    message = {
        "id": response.id,
        "error": {
            "code": response.error.code,
            "message": response.error.message,
            "data": response.error.data,
        },
    }

    return compose_text(message)


def compose_result(response: JsonRpcSuccessResponse) -> bytes | str:
    message = {
        "id": response.id,
        "result": response.result,
    }

    if response.binary:
        return compose_binary(message, response.binary)

    return compose_text(message)


def compose_response(response: JsonRpcResponse) -> bytes | str:
    if isinstance(response, JsonRpcErrorResponse):
        return compose_error(response)

    return compose_result(response)


def mock_connection(responses: list[JsonRpcResponse]) -> tuple[Connection, MockWebSocket]:
    packets = [compose_response(response) for response in responses]

    websocket = MockWebSocket(packets)

    return Connection(websocket), websocket
