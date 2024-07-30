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
from dataclasses import dataclass
from types import UnionType
from typing import Any, cast, get_args, get_origin

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


def has_type(value: Any, t: Any) -> bool:
    if t is Any:
        return True

    if t is None:
        return value is None

    origin = get_origin(t)

    if origin is UnionType:
        args = get_args(t)
        return any(has_type(value, arg) for arg in args)

    if origin is None:
        origin = t

    if not isinstance(value, origin):
        return False

    if origin is list:
        items = cast(list, value)
        (arg,) = get_args(t)
        return all(has_type(item, arg) for item in items)

    if origin is dict:
        items = cast(dict, value)
        keytype, valuetype = get_args(t)
        return all(
            has_type(key, keytype) and has_type(item, valuetype)
            for key, item in items.items()
        )

    return True


def check_type(value: Any, t: Any) -> None:
    if not has_type(value, t):
        raise TypeError("Invalid type in JSON-RPC message")


def try_get(message: dict[str, Any], key: str, t: Any, default: Any = None) -> Any:
    value = message.get(key, default)

    check_type(value, t)

    return value


def get(message: dict[str, Any], key: str, t: Any) -> Any:
    if key not in message:
        raise KeyError(f"Missing mandatory key in JSON-RPC message {key}")

    value = message[key]
    check_type(value, t)

    return value


def deserialize_result(
    message: dict[str, Any], binary: bytes = b""
) -> JsonRpcSuccessResponse:
    response_id = get(message, "id", int | str)

    return JsonRpcSuccessResponse(
        id=response_id,
        result=message["result"],
        binary=binary,
    )


def deserialize_error(message: dict[str, Any]) -> JsonRpcErrorResponse:
    error: dict[str, Any] = get(message, "error", dict[str, Any])

    return JsonRpcErrorResponse(
        id=try_get(message, "id", int | str | None, None),
        error=JsonRpcError(
            code=get(error, "code", int),
            message=get(error, "message", str),
            data=error.get("data"),
        ),
    )


def deserialize_response(
    message: dict[str, Any], binary: bytes = b""
) -> JsonRpcResponse:
    if "result" in message:
        return deserialize_result(message, binary)

    if "error" not in message:
        raise ValueError("Invalid JSON-RPC message without 'result' and 'error'")

    if binary:
        raise ValueError("Invalid error message with binary")

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
