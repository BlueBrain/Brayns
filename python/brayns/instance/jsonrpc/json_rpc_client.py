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
from typing import Union

from brayns.instance.jsonrpc.json_rpc_dispatcher import JsonRpcDispatcher
from brayns.instance.jsonrpc.json_rpc_handler import JsonRpcHandler
from brayns.instance.jsonrpc.json_rpc_manager import JsonRpcManager
from brayns.instance.jsonrpc.json_rpc_request import JsonRpcRequest
from brayns.instance.jsonrpc.json_rpc_task import JsonRpcTask
from brayns.instance.websocket.web_socket import WebSocket


class JsonRpcClient:

    def __init__(self, websocket: WebSocket, logger: logging.Logger) -> None:
        self._websocket = websocket
        self._logger = logger
        self._manager = JsonRpcManager()
        self._handler = JsonRpcHandler(self._manager, logger)
        self._dispatcher = JsonRpcDispatcher(self._handler)

    def __contains__(self, id: Union[int, str]) -> bool:
        return id in self._manager

    def __enter__(self) -> 'JsonRpcClient':
        return self

    def __exit__(self, *_) -> None:
        self.disconnect()

    def disconnect(self) -> None:
        self._logger.info('Disconnecting from instance.')
        self._manager.cancel_all_tasks()
        self._websocket.close()

    def send(self, request: JsonRpcRequest) -> JsonRpcTask:
        self._logger.info('Sending request: %s.', request)
        data = request.to_json()
        self._websocket.send(data)
        if request.is_notification():
            return JsonRpcTask.from_result(None)
        return self._manager.add_task(request.id)

    def poll(self) -> None:
        self._logger.debug('Polling incoming messages.')
        data = self._websocket.receive()
        self._dispatcher.dispatch(data)
