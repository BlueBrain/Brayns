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

from typing import Any, Dict

from .request_error import RequestError
from .request_progress import RequestProgress
from .request_task import RequestTask


class RequestManager:

    def __init__(self) -> None:
        self._tasks: Dict[int, RequestTask] = {}

    def create_task(self, id: int) -> RequestTask:
        if id in self._tasks:
            raise RuntimeError('Request with same ID already running')
        task = RequestTask()
        self._tasks[id] = task
        return task

    def clear_tasks(self) -> None:
        for task in self._tasks.values():
            task.set_exception(
                RequestError('Client disconnected')
            )
        self._tasks.clear()

    def set_result(self, id: int, result: Any) -> None:
        task = self._tasks.get(id)
        if task is None:
            return
        task.set_result(result)
        del self._tasks[id]

    def set_error(self, id: int, error: RequestError) -> None:
        task = self._tasks.get(id)
        if task is None:
            return
        task.set_exception(error)
        del self._tasks[id]

    def add_progress(self, id: int, progress: RequestProgress) -> None:
        task = self._tasks.get(id)
        if task is None:
            return
        task.add_progress(progress)
