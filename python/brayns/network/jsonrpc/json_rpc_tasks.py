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

from collections.abc import Iterator

from .json_rpc_error import JsonRpcError
from .json_rpc_progress import JsonRpcProgress
from .json_rpc_reply import JsonRpcReply
from .json_rpc_task import JsonRpcTask


class JsonRpcTasks:
    def __init__(self) -> None:
        self._tasks: dict[int | str, JsonRpcTask] = {}

    def __len__(self) -> int:
        return len(self._tasks)

    def __iter__(self) -> Iterator[tuple[int | str, JsonRpcTask]]:
        yield from self._tasks.items()

    def __contains__(self, id: int | str) -> bool:
        return id in self._tasks

    def find(self, id: int | str) -> JsonRpcTask | None:
        return self._tasks.get(id)

    def create_task(self, id: int | str) -> JsonRpcTask:
        if id in self._tasks:
            raise ValueError("Request with same ID already running")
        task = JsonRpcTask()
        self._tasks[id] = task
        return task

    def add_reply(self, reply: JsonRpcReply) -> None:
        if reply.id is None:
            raise ValueError("No request ID in reply")
        task = self.find(reply.id)
        if task is None:
            return
        task.set_reply(reply)
        del self._tasks[reply.id]

    def add_error(self, error: JsonRpcError) -> None:
        if error.id is None:
            self._add_general_error(error)
            return
        task = self.find(error.id)
        if task is None:
            return
        task.set_error(error)
        del self._tasks[error.id]

    def add_progress(self, progress: JsonRpcProgress) -> None:
        task = self.find(progress.id)
        if task is None:
            return
        task.add_progress(progress)

    def _add_general_error(self, error: JsonRpcError) -> None:
        for task in self._tasks.values():
            task.set_error(error)
        self._tasks.clear()
