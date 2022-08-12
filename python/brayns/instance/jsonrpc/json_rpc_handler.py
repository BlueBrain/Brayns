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

from .json_rpc_error import JsonRpcError
from .json_rpc_listener import JsonRpcListener
from .json_rpc_progress import JsonRpcProgress
from .json_rpc_reply import JsonRpcReply
from .json_rpc_tasks import JsonRpcTasks
from .request_error import RequestError


class JsonRpcHandler(JsonRpcListener):

    def __init__(self, tasks: JsonRpcTasks, logger: logging.Logger) -> None:
        self._tasks = tasks
        self._logger = logger

    def on_reply(self, reply: JsonRpcReply) -> None:
        self._logger.info('JSON-RPC reply received: %s.', reply)
        self._logger.debug('Reply result: %s.', reply.result)
        self._tasks.add_result(reply.id, reply.result)

    def on_error(self, error: JsonRpcError) -> None:
        self._logger.info('JSON-RPC error received: %s.', error)
        self._tasks.add_error(error.id, error.error)

    def on_progress(self, progress: JsonRpcProgress) -> None:
        self._logger.info('JSON-RPC progress received: %s.', progress)
        self._tasks.add_progress(progress.id, progress.params)

    def on_invalid_message(self, data: str, e: Exception) -> None:
        self._logger.error('Invalid JSON-RPC message (%s): "%s".', e, data)
        error = RequestError(0, 'Invalid JSON-RPC message received')
        self._tasks.add_error(None, error)
