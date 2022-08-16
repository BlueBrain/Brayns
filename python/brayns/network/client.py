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

from __future__ import annotations

import logging

from .instance import Instance
from .jsonrpc.json_rpc_manager import JsonRpcManager
from .jsonrpc.json_rpc_request import JsonRpcRequest
from .jsonrpc.request_future import RequestFuture
from .websocket.web_socket import WebSocket


class Client(Instance):

    def __init__(self, websocket: WebSocket, logger: logging.Logger, manager: JsonRpcManager) -> None:
        self._websocket = websocket
        self._logger = logger
        self._manager = manager

    @property
    def connected(self) -> bool:
        return not self._websocket.closed

    def disconnect(self) -> None:
        self._logger.info('Disconnection from Brayns instance.')
        self._manager.clear()
        self._websocket.close()

    def is_running(self, id: int | str) -> bool:
        return self._manager.is_running(id)

    def send(self, request: JsonRpcRequest) -> RequestFuture:
        self._logger.info('Send JSON-RPC request: %s.', request)
        self._logger.debug('Request params: %s.', request.params)
        data = request.to_json()
        self._websocket.send_text(data)
        id = request.id
        if id is None:
            return RequestFuture.from_result(None)
        return RequestFuture(
            task=self._manager.create_task(id),
            cancel=lambda: self.cancel(id),
            poll=lambda: self.poll()
        )

    def poll(self, block: bool = True) -> None:
        self._logger.debug('Waiting for messages from Brayns instance.')
        self._websocket.poll(block)

    def cancel(self, id: int | str) -> None:
        self._logger.info('Cancel request with ID %s.', id)
        self.request('cancel', {'id': id})
