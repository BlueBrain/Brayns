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

from brayns.instance.jsonrpc.json_rpc_error import JsonRpcError
from brayns.instance.jsonrpc.json_rpc_manager import JsonRpcManager
from brayns.instance.jsonrpc.json_rpc_progress import JsonRpcProgress
from brayns.instance.jsonrpc.json_rpc_protocol import JsonRpcProtocol
from brayns.instance.jsonrpc.json_rpc_reply import JsonRpcReply


class JsonRpcHandler(JsonRpcProtocol):

    def __init__(self, manager: JsonRpcManager, logger: logging.Logger) -> None:
        self._manager = manager
        self._logger = logger

    def on_binary(self, data: bytes) -> None:
        self._logger.info('Binary frame of %s bytes received.', len(data))

    def on_reply(self, reply: JsonRpcReply) -> None:
        self._logger.info('Reply received: %s.', reply)
        self._manager.set_result(reply.id, reply.result)

    def on_error(self, error: JsonRpcError) -> None:
        self._logger.info('Error received: %s.', error)
        if error.is_global():
            self._manager.cancel_all_tasks()
            return
        self._manager.set_error(error.id, error.error)

    def on_progress(self, progress: JsonRpcProgress) -> None:
        self._logger.info('Progress received: %s.', progress)
        self._manager.add_progress(progress.id, progress.params)

    def on_invalid_frame(self, data: Union[bytes, str], e: Exception) -> None:
        self._logger.error('Invalid frame received (%s): %s', e, data)
