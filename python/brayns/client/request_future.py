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

from typing import Any, Callable, Generator

from .request_progress import RequestProgress
from .jsonrpc.json_rpc_task import JsonRpcTask


class RequestFuture:

    @staticmethod
    def for_notification() -> 'RequestFuture':
        return RequestFuture(
            cancel=lambda: None,
            receive=lambda: None,
            task=JsonRpcTask.from_result(None)
        )

    def __init__(
        self,
        cancel: Callable[[], None],
        receive: Callable[[], None],
        task: JsonRpcTask
    ) -> None:
        self._cancel = cancel
        self._receive = receive
        self._task = task

    def cancel(self) -> None:
        self._cancel()

    def get_result(self) -> Any:
        for _ in self.wait():
            pass
        return self._task.get_result()

    def wait(self) -> Generator[RequestProgress]:
        while not self._task.is_ready():
            self._receive()
            if self._task.has_progress():
                yield self._task.get_progress()
