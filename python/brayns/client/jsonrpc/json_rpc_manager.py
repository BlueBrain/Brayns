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
from typing import Tuple, Union

from ...utils.id_generator import IdGenerator
from .json_rpc_context import JsonRpcContext
from .json_rpc_error import JsonRpcError
from .json_rpc_progress import JsonRpcProgress
from .json_rpc_reply import JsonRpcReply
from .json_rpc_task import JsonRpcTask


class JsonRpcManager:

    def __init__(self, logger: logging.Logger) -> None:
        self._logger = logger
        self._generator = IdGenerator()
        self._context = JsonRpcContext()

    def clear_tasks(self) -> None:
        self._context.clear_tasks()

    def add_task(self) -> Tuple[int, JsonRpcTask]:
        id = self._generator.generate_new_id()
        return id, self._context.add_task(id)

    def on_binary(self, data: bytes) -> None:
        self._logger.debug('Binary frame of {} bytes received.', len(data))

    def on_reply(self, reply: JsonRpcReply) -> None:
        self._logger.debug('Reply received {}.', reply)
        self._context.set_result(reply.id, reply.result)
        self._generator.recycle_id(reply.id)

    def on_error(self, error: JsonRpcError) -> None:
        self._logger.debug('Error message received {}.', error)
        self._context.set_error(error.id, error.error)
        self._generator.recycle_id(error.id)

    def on_progress(self, progress: JsonRpcProgress) -> None:
        self._logger.debug('Progress message received {}.', progress)
        self._context.add_progress(progress.id, progress.params)

    def on_invalid_frame(self, data: Union[bytes, str], e: Exception) -> None:
        self._logger.error('Invalid message received {}.', data, exc_info=e)
