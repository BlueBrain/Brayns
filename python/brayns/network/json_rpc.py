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

from dataclasses import dataclass
from typing import Any

from brayns.utils.composing import compose
from brayns.utils.parsing import deserialize, parse

JsonRpcId = int | str


@dataclass
class JsonRpcRequest:
    method: str
    params: Any = None
    binary: bytes = b""
    id: JsonRpcId | None = None


@dataclass
class JsonRpcSuccessResponse:
    id: JsonRpcId
    result: Any
    binary: bytes = b""


@dataclass
class JsonRpcError(Exception):
    code: int
    message: str
    data: Any = None

    def __str__(self) -> str:
        return f"{self.message} (code = {self.code})"


@dataclass
class JsonRpcErrorResponse:
    id: JsonRpcId | None
    error: JsonRpcError


JsonRpcResponse = JsonRpcSuccessResponse | JsonRpcErrorResponse


def compose_request(request: JsonRpcRequest) -> bytes | str:
    message: dict[str, Any] = {
        "jsonrpc": "2.0",
        "method": request.method,
    }

    if request.params is not None:
        message["params"] = request.params

    if request.id is not None:
        message["id"] = request.id

    return compose(message, request.binary)


def parse_response(data: bytes | str) -> JsonRpcResponse:
    message, binary = parse(data)

    response = deserialize(message, JsonRpcResponse)

    if isinstance(response, JsonRpcSuccessResponse):
        response.binary = binary
        return response

    if binary:
        raise ValueError("Invalid JSON-RPC error with binary")

    return response
