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

from typing import Any, Iterator, Optional, Union

from brayns.instance.jsonrpc.json_rpc_task import JsonRpcTask
from brayns.instance.request_error import RequestError


class JsonRpcManager:

    def __init__(self) -> None:
        self._tasks: dict[Union[int, str], JsonRpcTask] = {}

    def __len__(self) -> int:
        return len(self._tasks)

    def __iter__(self) -> Iterator[tuple[Union[int, str], JsonRpcTask]]:
        yield from self._tasks.items()

    def __contains__(self, id: Union[int, str]) -> bool:
        return id in self._tasks

    def get_task(self, id: Union[int, str]) -> Optional[JsonRpcTask]:
        return self._tasks.get(id)

    def add_task(self, id: Union[int, str]) -> JsonRpcTask:
        if id in self._tasks:
            raise RuntimeError('Request with same ID already running')
        task = JsonRpcTask()
        self._tasks[id] = task
        return task

    def cancel_all_tasks(self) -> None:
        error = RequestError(0, 'Task cancelled from client side')
        for task in self._tasks.values():
            task.set_error(error)
        self._tasks.clear()

    def set_result(self, id: Union[int, str], result: Any) -> None:
        task = self.get_task(id)
        if task is None:
            return
        task.set_result(result)
        del self._tasks[id]

    def set_error(self, id: Union[int, str], error: RequestError) -> None:
        task = self.get_task(id)
        if task is None:
            return
        task.set_error(error)
        del self._tasks[id]
