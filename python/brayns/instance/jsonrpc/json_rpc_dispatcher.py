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
from typing import Union

from brayns.instance.jsonrpc.json_rpc_error import JsonRpcError
from brayns.instance.jsonrpc.json_rpc_progress import JsonRpcProgress
from brayns.instance.jsonrpc.json_rpc_protocol import JsonRpcProtocol
from brayns.instance.jsonrpc.json_rpc_reply import JsonRpcReply


class JsonRpcDispatcher:

    def __init__(self, protocol: JsonRpcProtocol) -> None:
        self._protocol = protocol

    def dispatch(self, data: Union[bytes, str]) -> None:
        try:
            self._dispatch(data)
        except Exception as e:
            self._protocol.on_invalid_frame(data, e)

    def _dispatch(self, data: Union[bytes, str]) -> None:
        if isinstance(data, bytes):
            self._protocol.on_binary(data)
            return
        self._dispatch_text(data)

    def _dispatch_text(self, data: str) -> None:
        message = self._parse(data)
        if self._dispatch_error(message):
            return
        if self._dispatch_reply(message):
            return
        if self._dispatch_progress(message):
            return
        raise ValueError('Unsupported JSON-RPC message')

    def _parse(self, data: str) -> dict:
        message = json.loads(data)
        if not isinstance(message, dict):
            raise ValueError('Message is not a JSON object')
        return message

    def _dispatch_error(self, message: dict) -> bool:
        if 'error' not in message:
            return False
        error = JsonRpcError.from_dict(message)
        self._protocol.on_error(error)
        return True

    def _dispatch_reply(self, message: dict) -> bool:
        if 'result' not in message:
            return False
        reply = JsonRpcReply.from_dict(message)
        self._protocol.on_reply(reply)
        return True

    def _dispatch_progress(self, message: dict) -> bool:
        if 'id' in message:
            return False
        progress = JsonRpcProgress.from_dict(message)
        self._protocol.on_progress(progress)
        return True
