# Copyright (c) 2015-2023 EPFL/Blue Brain Project
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

from .json_rpc_error import deserialize_error
from .json_rpc_listener import JsonRpcListener
from .json_rpc_progress import deserialize_progress
from .json_rpc_reply import deserialize_reply, deserialize_reply_from_binary


class JsonRpcDispatcher:
    def __init__(self, listener: JsonRpcListener) -> None:
        self._listener = listener

    def dispatch_binary(self, data: bytes) -> None:
        try:
            reply = deserialize_reply_from_binary(data)
            self._listener.on_reply(reply)
        except Exception as e:
            self._listener.on_invalid_message(e)

    def dispatch_text(self, data: str) -> None:
        try:
            self._dispatch(data)
        except Exception as e:
            self._listener.on_invalid_message(e)

    def _dispatch(self, data: str) -> None:
        message = self._parse(data)
        if self._dispatch_error(message):
            return
        if self._dispatch_reply(message):
            return
        if self._dispatch_progress(message):
            return
        raise ValueError("Unsupported JSON-RPC message")

    def _parse(self, data: str) -> dict[str, Any]:
        message = json.loads(data)
        if not isinstance(message, dict):
            raise ValueError("Message is not a JSON object")
        return message

    def _dispatch_error(self, message: dict[str, Any]) -> bool:
        if "error" not in message:
            return False
        error = deserialize_error(message)
        self._listener.on_error(error)
        return True

    def _dispatch_reply(self, message: dict[str, Any]) -> bool:
        if "result" not in message:
            return False
        reply = deserialize_reply(message)
        self._listener.on_reply(reply)
        return True

    def _dispatch_progress(self, message: dict[str, Any]) -> bool:
        if "id" in message:
            return False
        progress = deserialize_progress(message)
        self._listener.on_progress(progress)
        return True
