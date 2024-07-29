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
    result: Any = None
    binary: bytes = b""


@dataclass
class JsonRpcError(Exception):
    code: int
    message: str
    data: Any = None


@dataclass
class JsonRpcErrorResponse:
    id: JsonRpcId | None
    error: JsonRpcError


JsonRpcResponse = JsonRpcSuccessResponse | JsonRpcErrorResponse


def serialize_request(request: JsonRpcRequest) -> dict[str, Any]:
    message = {
        "jsonrpc": "2.0",
        "method": request.method,
        "params": request.params,
    }

    if request.id is not None:
        message["id"] = request.id

    return message


def compose_text(message: Any) -> str:
    return json.dumps(message, separators=(",", ":"))


def compose_binary(message: Any, binary: bytes) -> bytes:
    text = compose_text(message)
    json_part = text.encode()

    header = len(json_part).to_bytes(4, byteorder="little", signed=False)

    return header + json_part + binary


def compose_request(request: JsonRpcRequest) -> bytes | str:
    message = serialize_request(request)

    if request.binary:
        return compose_binary(message, request.binary)

    return compose_text(message)


T = TypeVar("T")


def _get(message: dict[str, Any], key: str, t: type[T]) -> T:
    value = message[key]

    if not isinstance(value, t):
        raise ValueError("Invalid JSON-RPC message value type")

    return value


def _get_id(message: dict[str, Any]) -> JsonRpcId | None:
    id = message.get("id")

    if id is None:
        return None

    if isinstance(id, int | str):
        return id

    raise ValueError("Invalid JSON-RPC ID type")


def deserialize_result(message: dict[str, Any], binary: bytes = b"") -> JsonRpcSuccessResponse:
    response_id = _get_id(message)

    if response_id is None:
        raise ValueError("Result responses must have an ID")

    return JsonRpcSuccessResponse(
        id=response_id,
        result=message["result"],
        binary=binary,
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


def deserialize_response(message: dict[str, Any], binary: bytes = b"") -> JsonRpcResponse:
    if "result" in message:
        return deserialize_result(message, binary)

    if "error" not in message:
        raise ValueError("Invalid JSON-RPC message")

    if binary:
        raise ValueError("Invalid binary in error message")

    return deserialize_error(message)


def parse_binary(data: bytes) -> tuple[Any, bytes]:
    size = len(data)

    if size < 4:
        raise ValueError("Invalid binary frame with header < 4 bytes")

    header = data[:4]

    json_size = int.from_bytes(header, byteorder="little", signed=False)

    if json_size > size - 4:
        raise ValueError(f"Invalid JSON size: {json_size} > {size - 4}")

    json_part = data[4 : 4 + json_size].decode("utf-8")
    message = json.loads(json_part)

    binary_part = data[4 + json_size :]

    return message, binary_part


def parse_response(data: bytes | str) -> JsonRpcResponse:
    if isinstance(data, bytes):
        message, binary = parse_binary(data)

        return deserialize_response(message, binary)

    message = json.loads(data)

    return deserialize_response(message)
