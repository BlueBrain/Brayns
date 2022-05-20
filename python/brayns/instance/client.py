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

import logging
from typing import Any

from brayns.instance.instance import Instance
from brayns.instance.jsonrpc.json_rpc_id import JsonRpcId
from brayns.instance.jsonrpc.json_rpc_manager import JsonRpcManager
from brayns.instance.jsonrpc.json_rpc_request import JsonRpcRequest
from brayns.instance.request_future import RequestFuture
from brayns.instance.websocket.web_socket import WebSocket
from brayns.instance.websocket.web_socket_error import WebSocketError


class Client(Instance):

    def __init__(self, websocket: WebSocket, logger: logging.Logger) -> None:
        self._websocket = websocket
        self._logger = logger
        self._manager = JsonRpcManager(logger)

    def disconnect(self) -> None:
        self._logger.info('Disconnection from Brayns instance.')
        self._manager.clear()
        self._websocket.close()

    def task(self, method: str, params: Any = None) -> RequestFuture:
        id = 0
        while self._manager.is_running(id):
            id += 1
        request = JsonRpcRequest(id, method, params)
        return self.send(request)

    def send(self, request: JsonRpcRequest) -> RequestFuture:
        self._logger.info('Sending request %s.', request)
        data = request.to_json()
        self._websocket.send(data)
        return RequestFuture(
            task=self._manager.create_task(request.id),
            cancel=lambda: self.cancel(request.id),
            poll=self.poll
        )

    def poll(self) -> None:
        self._logger.debug('Polling messages from Brayns instance.')
        data = self._websocket.receive()
        if isinstance(data, bytes):
            self._logger.info('Binary frame received of %d bytes.', len(data))
            return
        if isinstance(data, str):
            self._logger.info('Text frame received: "%s".', data)
            self._manager.process_message(data)
            return
        raise WebSocketError(f'Invalid frame type received: {type(data)}')

    def cancel(self, id: JsonRpcId) -> None:
        self._logger.info('Cancel request with ID %s.', id)
        self.request('cancel', {'id': id})
