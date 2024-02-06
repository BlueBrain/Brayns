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
from typing import Any

import brayns


def mock_error() -> brayns.JsonRpcError:
    return brayns.JsonRpcError(
        id=0,
        code=1,
        message="test",
        data=[1, 2, 3],
    )


def mock_error_message() -> dict[str, Any]:
    return {
        "jsonrpc": "2.0",
        "id": 0,
        "error": {
            "code": 1,
            "message": "test",
            "data": [1, 2, 3],
        },
    }


def mock_error_data() -> str:
    return json.dumps(mock_error_message(), sort_keys=True)


def mock_progress() -> brayns.JsonRpcProgress:
    return brayns.JsonRpcProgress(
        id=0,
        operation="test",
        amount=0.5,
    )


def mock_progress_message() -> dict[str, Any]:
    return {
        "jsonrpc": "2.0",
        "params": {
            "id": 0,
            "operation": "test",
            "amount": 0.5,
        },
    }


def mock_progress_data() -> str:
    return json.dumps(mock_progress_message(), sort_keys=True)


def mock_reply(binary: bool = False) -> brayns.JsonRpcReply:
    return brayns.JsonRpcReply(id=0, result=123, binary=b"123" if binary else b"")


def mock_reply_message() -> dict[str, Any]:
    return {
        "jsonrpc": "2.0",
        "id": 0,
        "result": 123,
    }


def mock_reply_text() -> str:
    return json.dumps(mock_reply_message(), sort_keys=True)


def mock_reply_binary() -> bytes:
    text = mock_reply_text().encode("utf-8")
    size = len(text).to_bytes(4, byteorder="little", signed=False)
    binary = b"123"
    return b"".join([size, text, binary])


def mock_request(binary: bool = False) -> brayns.JsonRpcRequest:
    return brayns.JsonRpcRequest(
        id=0, method="test", params=123, binary=b"123" if binary else b""
    )


def mock_request_message() -> dict[str, Any]:
    return {
        "jsonrpc": "2.0",
        "id": 0,
        "method": "test",
        "params": 123,
    }


def mock_request_text() -> str:
    return json.dumps(mock_request_message(), sort_keys=True)


def mock_request_binary() -> bytes:
    text = mock_request_text().encode("utf-8")
    size = len(text).to_bytes(4, byteorder="little", signed=False)
    binary = b"123"
    return b"".join([size, text, binary])
