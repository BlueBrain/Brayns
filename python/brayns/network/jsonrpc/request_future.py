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

from __future__ import annotations

from collections.abc import Callable, Iterator
from typing import Any

from .json_rpc_task import JsonRpcTask
from .messages import RequestProgress


class RequestFuture:
    """Future used to monitor a running request.

    Can be iterated to yield ``RequestProgress`` if the request send any.

    Use ``wait_for_result`` to block until the request is over.

    Use ``poll`` to refresh the state of the request.

    Use ``cancel`` to send a message to cancel the request.

    Future ready and progress state can also be queried.
    """

    @staticmethod
    def from_result(result: Any) -> RequestFuture:
        return RequestFuture(
            task=JsonRpcTask.from_result(result),
            cancel=lambda: None,
            poll=lambda: None
        )

    def __init__(self, task: JsonRpcTask, cancel: Callable[[], None], poll: Callable[[], None]) -> None:
        self._task = task
        self._cancel = cancel
        self._poll = poll

    def __iter__(self) -> Iterator[RequestProgress]:
        while True:
            while self.has_progress():
                yield self.get_progress()
            if self.is_ready():
                return
            self.poll()

    def is_ready(self) -> bool:
        return self._task.is_ready()

    def has_progress(self) -> bool:
        return self._task.has_progress()

    def get_progress(self) -> RequestProgress:
        return self._task.get_progress()

    def wait_for_result(self) -> Any:
        for _ in self:
            pass
        return self._task.get_result()

    def poll(self) -> None:
        self._poll()

    def cancel(self) -> None:
        self._cancel()
