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

from ..request.request_manager import RequestManager
from ..request.request_task import RequestTask
from .json_rpc_error import JsonRpcError
from .json_rpc_reply import JsonRpcReply
from .json_rpc_progress import JsonRpcProgress


class JsonRpcManager:

    def __init__(self) -> None:
        self._manager = RequestManager()

    def create_task(self, id: int) -> RequestTask:
        return self._manager.create_task(id)

    def clear_tasks(self) -> None:
        self._manager.clear_tasks()

    def on_reply(self, reply: JsonRpcReply) -> None:
        self._manager.set_result(reply.id, reply.result)

    def on_error(self, error: JsonRpcError) -> None:
        self._manager.set_error(error.id, error.error)

    def on_progress(self, progress: JsonRpcProgress) -> None:
        self._manager.add_progress(progress.id, progress.params)

    def on_invalid_message(self, data: str, e: Exception) -> None:
        print(f'Invalid message received: {data} {e}')
