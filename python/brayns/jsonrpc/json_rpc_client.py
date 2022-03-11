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

from ..websocket.web_socket import WebSocket
from ..websocket.web_socket_client import WebSocketClient
from .json_rpc_future import JsonRpcFuture
from .json_rpc_handler import JsonRpcHandler
from .json_rpc_manager import JsonRpcManager
from .json_rpc_request import JsonRpcRequest


class JsonRpcClient:

    def __init__(self, websocket: WebSocket) -> None:
        self._manager = JsonRpcManager()
        self._client = WebSocketClient(
            listener=JsonRpcHandler(self._manager),
            websocket=websocket
        )

    def disconnect(self) -> None:
        self._manager.on_disconnect()
        self._client.disconnect()

    def send(self, request: JsonRpcRequest) -> JsonRpcFuture:
        future = self._manager.add_request(request)
        self._client.send_text(
            request.to_json()
        )
        return future
