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
from typing import Any, Union

from ..request_future import RequestFuture
from ..websocket.web_socket_protocol import WebSocketProtocol
from .json_rpc_dispatcher import JsonRpcDispatcher
from .json_rpc_manager import JsonRpcManager
from .json_rpc_request import JsonRpcRequest


class JsonRpcClient:

    def __init__(
        self,
        logger: logging.Logger,
        websocket: WebSocketProtocol
    ) -> None:
        self._logger = logger
        self._websocket = websocket
        self._manager = JsonRpcManager(self._logger)
        self._dispatcher = JsonRpcDispatcher(self._manager)

    def disconnect(self) -> None:
        self._logger.debug('Disconnecting from server')
        self._websocket.close()
        self._manager.clear_tasks()

    def receive(self) -> None:
        self._dispatcher.dispatch(
            self._websocket.receive()
        )

    def send(self, method: str, params: Any = None) -> RequestFuture:
        id, task = self._manager.add_task()
        self._websocket.send(
            JsonRpcRequest(id, method, params).to_json()
        )
        return RequestFuture(
            cancel=lambda: self._cancel(id),
            receive=self.receive,
            task=task
        )

    def _cancel(self, id: Union[int, str]) -> None:
        self.send('cancel', {'id': id}).get_result()
