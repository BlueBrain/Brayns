# Copyright (c) 2015-2023 EPFL/Blue Brain Project
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

from .json_rpc_dispatcher import JsonRpcDispatcher
from .json_rpc_error import JsonRpcError
from .json_rpc_handler import JsonRpcHandler
from .json_rpc_task import JsonRpcTask
from .json_rpc_tasks import JsonRpcTasks


class JsonRpcManager:
    def __init__(self, logger: logging.Logger) -> None:
        self._logger = logger
        self._tasks = JsonRpcTasks()
        listener = JsonRpcHandler(self._tasks, self._logger)
        self._dispatcher = JsonRpcDispatcher(listener)

    def is_running(self, id: int | str) -> bool:
        return id in self._tasks

    def clear(self) -> None:
        self._logger.debug("Clear all JSON-RPC tasks.")
        error = JsonRpcError.general("Disconnection from client side")
        self._tasks.add_error(error)

    def create_task(self, id: int | str) -> JsonRpcTask:
        self._logger.debug("Create JSON-RPC task with ID %s.", id)
        return self._tasks.create_task(id)

    def process_binary(self, data: bytes) -> None:
        self._logger.debug("Processing JSON-RPC binary message: %s.", data)
        self._dispatcher.dispatch_binary(data)

    def process_text(self, data: str) -> None:
        self._logger.debug("Processing JSON-RPC text message: %s.", data)
        self._dispatcher.dispatch_text(data)
