# Copyright (c) 2015-2022 EPFL/Blue Brain Project
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

from .json_rpc_listener import JsonRpcListener
from .messages import (
    JsonRpcError,
    JsonRpcProgress,
    JsonRpcReply,
    RequestError,
    RequestProgress,
)


class JsonRpcDispatcher:

    def __init__(self, listener: JsonRpcListener) -> None:
        self._listener = listener

    def dispatch(self, data: str) -> None:
        try:
            self._dispatch(data)
        except Exception as e:
            self._listener.on_invalid_message(data, e)

    def _dispatch(self, data: str) -> None:
        message = self._parse(data)
        if self._dispatch_error(message):
            return
        if self._dispatch_reply(message):
            return
        if self._dispatch_progress(message):
            return
        raise ValueError('Unsupported JSON-RPC message')

    def _parse(self, data: str) -> dict[str, Any]:
        message = json.loads(data)
        if not isinstance(message, dict):
            raise ValueError('Message is not a JSON object')
        return message

    def _dispatch_error(self, message: dict[str, Any]) -> bool:
        if 'error' not in message:
            return False
        error = _deserialize_error(message)
        self._listener.on_error(error)
        return True

    def _dispatch_reply(self, message: dict[str, Any]) -> bool:
        if 'result' not in message:
            return False
        reply = _deserialize_reply(message)
        self._listener.on_reply(reply)
        return True

    def _dispatch_progress(self, message: dict[str, Any]) -> bool:
        if 'id' in message:
            return False
        progress = _deserialize_progress(message)
        self._listener.on_progress(progress)
        return True


def _deserialize_error(message: dict[str, Any]) -> JsonRpcError:
    error: dict[str, Any] = message['error']
    return JsonRpcError(
        id=message.get('id'),
        error=RequestError(
            code=error['code'],
            message=error['message'],
            data=error.get('data'),
        ),
    )


def _deserialize_reply(message: dict[str, Any]) -> JsonRpcReply:
    return JsonRpcReply(
        id=message['id'],
        result=message['result'],
    )


def _deserialize_progress(message: dict[str, Any]) -> JsonRpcProgress:
    params = message['params']
    return JsonRpcProgress(
        id=params['id'],
        params=RequestProgress(
            operation=params['operation'],
            amount=params['amount'],
        ),
    )
