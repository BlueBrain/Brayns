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

from .json_rpc_error import JsonRpcError
from .json_rpc_listener import JsonRpcListener
from .json_rpc_progress import JsonRpcProgress
from .json_rpc_reply import JsonRpcReply


class JsonRpcDispatcher:

    def __init__(self, listener: JsonRpcListener) -> None:
        self._listener = listener

    def dispatch(self, data: str) -> None:
        try:
            self._dispatch(data)
        except Exception as e:
            self._listener.on_invalid_message(data, e)

    def _dispatch(self, data: str) -> None:
        obj = self._parse(data)
        if self._dispatch_error(obj):
            return
        if self._dispatch_reply(obj):
            return
        if self._dispatch_progress(obj):
            return
        raise ValueError('Unsupported JSON-RPC message')

    def _parse(self, data: str) -> dict[str, Any]:
        obj = json.loads(data)
        if not isinstance(obj, dict):
            raise ValueError('Message is not a JSON object')
        return obj

    def _dispatch_error(self, obj: dict[str, Any]) -> bool:
        if 'error' not in obj:
            return False
        error = JsonRpcError.from_dict(obj)
        self._listener.on_error(error)
        return True

    def _dispatch_reply(self, obj: dict[str, Any]) -> bool:
        if 'result' not in obj:
            return False
        reply = JsonRpcReply.from_dict(obj)
        self._listener.on_reply(reply)
        return True

    def _dispatch_progress(self, obj: dict[str, Any]) -> bool:
        if 'id' in obj:
            return False
        progress = JsonRpcProgress.from_dict(obj)
        self._listener.on_progress(progress)
        return True
