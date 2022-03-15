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

from typing import Any, Dict, Union

from ..request_error import RequestError
from ..request_progress import RequestProgress
from .json_rpc_task import JsonRpcTask


class JsonRpcContext:

    def __init__(self) -> None:
        self._tasks: Dict[int, JsonRpcTask] = {}

    def add_task(self, id: Union[int, str]) -> JsonRpcTask:
        if id is None:
            raise RuntimeError('Cannot create a task for a notification')
        if id in self._tasks:
            raise RuntimeError('Request with same ID already running')
        task = JsonRpcTask()
        self._tasks[id] = task
        return task

    def clear_tasks(self) -> None:
        for task in self._tasks.values():
            task.set_error(
                RequestError('Task has been stopped on client side')
            )
        self._tasks.clear()

    def set_result(
        self,
        id: Union[int, str],
        result: Any
    ) -> None:
        task = self._tasks.get(id)
        if task is None:
            return
        task.set_result(result)
        del self._tasks[id]

    def set_error(
        self,
        id: Union[int, str],
        error: RequestError
    ) -> None:
        task = self._tasks.get(id)
        if task is None:
            return
        task.set_error(error)
        del self._tasks[id]

    def add_progress(
        self,
        id: Union[int, str],
        progress: RequestProgress
    ) -> None:
        task = self._tasks.get(id)
        if task is None:
            return
        task.add_progress(progress)
