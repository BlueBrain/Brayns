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

from ..websocket.web_socket_protocol import WebSocketProtocol
from .json_rpc_dispatcher import JsonRpcDispatcher
from .json_rpc_request import JsonRpcRequest
from ...utils.id_generator import IdGenerator
from ..request_future import RequestFuture
from .json_rpc_manager import JsonRpcManager
from .json_rpc_error import JsonRpcError
from .json_rpc_progress import JsonRpcProgress
from .json_rpc_reply import JsonRpcReply


class JsonRpcClient:

    def __init__(
        self,
        logger: logging.Logger,
        websocket: WebSocketProtocol
    ) -> None:
        self._logger = logger
        self._websocket = websocket
        self._generator = IdGenerator()
        self._manager = JsonRpcManager()
        self._dispatcher = JsonRpcDispatcher(self)

    def disconnect(self) -> None:
        self._logger.debug('Disconnecting from renderer')
        self._websocket.disconnect()
        self._manager.clear_tasks()

    def receive(self) -> None:
        self._dispatcher.dispatch(
            self._websocket.receive()
        )

    def send(self, method: str, params: Any = None) -> RequestFuture:
        id = self._generator.generate_new_id()
        task = self._manager.add_task(id)
        self._websocket.send(
            JsonRpcRequest(
                id=id,
                method=method,
                params=params
            ).to_json()
        )
        return RequestFuture(
            cancel=lambda: self.cancel(id),
            receive=self.receive,
            task=task
        )

    def cancel(self, id: Union[int, str]) -> None:
        self.send('cancel', {'id': id})

    def on_binary(self, data: bytes) -> None:
        self._logger.debug('Binary frame of {} bytes received.', len(data))

    def on_reply(self, reply: JsonRpcReply) -> None:
        self._logger.debug('Reply received {}.', reply)
        self._manager.set_result(reply.id, reply.result)
        self._generator.recycle_id(reply.id)

    def on_error(self, error: JsonRpcError) -> None:
        self._logger.debug('Error message received {}.', error)
        self._manager.set_error(error.id, error.error)
        self._generator.recycle_id(error.id)

    def on_progress(self, progress: JsonRpcProgress) -> None:
        self._logger.debug('Progress message received {}.', progress)
        self._manager.add_progress(progress.id, progress.params)

    def on_invalid_frame(self, data: Union[bytes, str], e: Exception) -> None:
        self._logger.error('Invalid message received {}.', data, exc_info=e)
