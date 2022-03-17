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

from ..request_error import RequestError
from ..websocket.web_socket_protocol import WebSocketProtocol
from .json_rpc_dispatcher import JsonRpcDispatcher
from .json_rpc_handler import JsonRpcHandler
from .json_rpc_manager import JsonRpcManager
from .json_rpc_request import JsonRpcRequest
from .json_rpc_task import JsonRpcTask


class JsonRpcClient:

    def __init__(
        self,
        websocket: WebSocketProtocol,
        logger: logging.Logger
    ) -> None:
        self._websocket = websocket
        self._logger = logger
        self._manager = JsonRpcManager()
        self._dispatcher = JsonRpcDispatcher(
            JsonRpcHandler(self._manager, self._logger)
        )

    def __enter__(self) -> 'JsonRpcClient':
        return self

    def __exit__(self, *_) -> None:
        self.disconnect()

    def disconnect(self) -> None:
        self._logger.debug('Disconnection from JSON-RPC server.')
        self._websocket.close()
        self._manager.cancel_all_tasks(
            RequestError('Disconnection from client side')
        )

    def poll(self) -> None:
        self._logger.debug('Poll incoming JSON-RPC messages.')
        self._dispatcher.dispatch(
            self._websocket.receive()
        )

    def send(self, request: JsonRpcRequest) -> JsonRpcTask:
        self._logger.debug('Send JSON-RPC request: {}.', request)
        self._websocket.send(request.to_json())
        if request.is_notification():
            return JsonRpcTask.from_result(None)
        return self._manager.add_task(request.id)

    def get_active_tasks(self) -> JsonRpcManager:
        return self._manager
