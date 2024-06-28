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
import json
from typing import Any, TypeVar

JsonRpcId = int | str | None


@dataclass
class JsonRpcRequest:
    id: JsonRpcId
    method: str
    params: Any = None
    binary: bytes = b""


@dataclass
class JsonRpcResponse:
    id: JsonRpcId
    result: Any = None
    binary: bytes = b""


@dataclass
class JsonRpcError(Exception):
    code: int
    message: str
    data: Any = None


@dataclass
class JsonRpcErrorResponse:
    id: JsonRpcId
    error: JsonRpcError


def serialize_request(request: JsonRpcRequest) -> dict[str, Any]:
    return {
        "jsonrpc": "2.0",
        "id": request.id,
        "method": request.method,
        "params": request.params,
    }


def compose_text_request(request: JsonRpcRequest) -> str:
    message = serialize_request(request)
    return json.dumps(message, separators=(",", ":"))


def compose_binary_request(request: JsonRpcRequest) -> bytes:
    json_part = compose_text_request(request).encode()

    header = len(json_part).to_bytes(4, byteorder="little", signed=False)

    binary_part = request.binary

    return b"".join([header, json_part, binary_part])


def compose_request(request: JsonRpcRequest) -> bytes | str:
    if request.binary:
        return compose_binary_request(request)
    return compose_text_request(request)


T = TypeVar("T")


def _get(message: dict[str, Any], key: str, t: type[T]) -> T:
    value = message[key]

    if not isinstance(value, t):
        raise ValueError("Invalid JSON-RPC message value type")

    return value


def _get_id(message: dict[str, Any]) -> JsonRpcId:
    id = message.get("id")

    if id is None:
        return None

    if isinstance(id, int | str):
        return id

    raise ValueError("Invalid JSON-RPC ID type")


def deserialize_result(message: dict[str, Any], binary: bytes = b"") -> JsonRpcResponse:
    return JsonRpcResponse(
        id=_get_id(message),
        result=message["result"],
    )


def deserialize_error(message: dict[str, Any]) -> JsonRpcErrorResponse:
    error = _get(message, "error", dict)

    return JsonRpcErrorResponse(
        id=_get_id(message),
        error=JsonRpcError(
            code=_get(error, "code", int),
            message=_get(error, "message", str),
            data=error.get("data"),
        ),
    )


def deserialize_response(message: dict[str, Any], binary: bytes = b"") -> JsonRpcResponse | JsonRpcErrorResponse:
    if "result" in message:
        return deserialize_result(message, binary)

    if "error" not in message:
        raise ValueError("Invalid JSON-RPC message")

    if binary:
        raise ValueError("Invalid binary in error message")

    return deserialize_error(message)


def parse_text_response(data: str) -> JsonRpcResponse | JsonRpcErrorResponse:
    message = json.loads(data)
    return deserialize_response(message)


def parse_binary_response(data: bytes) -> JsonRpcResponse | JsonRpcErrorResponse:
    size = len(data)

    if size < 4:
        raise ValueError("Invalid binary frame header < 4 bytes")

    header = data[:4]

    json_size = int.from_bytes(header, byteorder="little", signed=False)
    json_part = data[4 : 4 + json_size].decode("utf-8")
    message = json.loads(json_part)

    binary_part = data[4 + json_size :]

    return deserialize_response(message, binary_part)


def parse_response(data: bytes | str) -> JsonRpcResponse | JsonRpcErrorResponse:
    if isinstance(data, bytes):
        return parse_binary_response(data)

    if isinstance(data, str):
        return parse_text_response(data)

    raise TypeError("Invalid data type")
