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

from ..request.request import Request
from ..request.request_future import RequestFuture
from ..utils.id_generator import IdGenerator
from ..websocket.web_socket_client import WebSocketClient
from ..websocket.web_socket_client_factory import WebSocketClientFactory
from .json_rpc_handler import JsonRpcHandler
from .json_rpc_manager import JsonRpcManager
from .json_rpc_request import JsonRpcRequest


class JsonRpcClient:

    def __init__(self, factory: WebSocketClientFactory) -> None:
        self._generator = IdGenerator()
        self._manager = JsonRpcManager()
        self._client = WebSocketClient(
            listener=JsonRpcHandler(self._manager),
            factory=factory
        )

    def disconnect(self) -> None:
        self._client.disconnect()
        self._manager.clear_tasks()

    def send(self, request: Request) -> RequestFuture:
        id = self._generator.generate_new_id()
        task = self._manager.create_task(id)
        self._client.send_text(
            JsonRpcRequest.from_request(id, request).to_json()
        )
        return RequestFuture(
            task=task,
            cancel=lambda: self.send(Request.to_cancel(id))
        )
