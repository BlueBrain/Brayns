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

from typing import Dict, Union

from json_rpc_task import JsonRpcTask

from .json_rpc_error import JsonRpcError
from .json_rpc_future import JsonRpcFuture
from .json_rpc_progress import JsonRpcProgress
from .json_rpc_reply import JsonRpcReply
from .json_rpc_request import JsonRpcRequest
from .request_error import RequestError


class JsonRpcManager:

    def __init__(self) -> None:
        self._tasks: Dict[Union[int, str], JsonRpcTask] = {}

    def add_request(self, request: JsonRpcRequest) -> JsonRpcFuture:
        if request.is_notification():
            return JsonRpcFuture.for_notifications()
        task = JsonRpcTask()
        self._tasks[request.id] = task
        return JsonRpcFuture(
            task=task,
            on_cancel=lambda: self.on_cancel(request.id)
        )

    def on_cancel(self, request: JsonRpcRequest) -> None:
        del self._tasks[request.id]

    def on_disconnect(self) -> None:
        for task in self._tasks.values():
            task.set_exception(
                RequestError('Client disconnected')
            )
        self._tasks.clear()

    def on_reply(self, reply: JsonRpcReply) -> None:
        task = self._tasks.get(reply.id)
        if task is None:
            return
        task.set_result(reply.result)

    def on_error(self, error: JsonRpcError) -> None:
        task = self._tasks.get(error.id)
        if task is None:
            return
        task.set_exception(error.to_exception())

    def on_progress(self, progress: JsonRpcProgress) -> None:
        task = self._tasks.get(progress.id)
        if task is None:
            return
        task.add_progress(progress.to_progress())

    def on_invalid_message(self, data: str, e: Exception) -> None:
        print(f'Invalid message received: {data} {e}')
