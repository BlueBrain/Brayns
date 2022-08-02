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

from brayns.instance.jsonrpc.json_rpc_dispatcher import JsonRpcDispatcher
from brayns.instance.jsonrpc.json_rpc_handler import JsonRpcHandler
from brayns.instance.jsonrpc.json_rpc_id import JsonRpcId
from brayns.instance.jsonrpc.json_rpc_task import JsonRpcTask
from brayns.instance.jsonrpc.json_rpc_tasks import JsonRpcTasks
from brayns.instance.request_error import RequestError


class JsonRpcManager:

    def __init__(self, logger: logging.Logger) -> None:
        self._logger = logger
        self._tasks = JsonRpcTasks()
        self._dispatcher = JsonRpcDispatcher(
            listener=JsonRpcHandler(self._tasks, self._logger)
        )

    def is_running(self, id: JsonRpcId) -> bool:
        return id in self._tasks

    def clear(self) -> None:
        self._logger.debug('Clear all JSON-RPC tasks.')
        error = RequestError(0, 'Disconnection from client side')
        self._tasks.add_global_error(error)

    def create_task(self, id: JsonRpcId) -> JsonRpcTask:
        self._logger.debug('Create JSON-RPC task with ID %s.', id)
        return self._tasks.create_task(id)

    def process_message(self, data: str) -> None:
        self._logger.debug('Processing JSON-RPC message: %s.', data)
        self._dispatcher.dispatch(data)
