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

from collections.abc import Callable, Iterator

from .json_rpc_progress import JsonRpcProgress
from .json_rpc_reply import JsonRpcReply
from .json_rpc_task import JsonRpcTask


class JsonRpcFuture:
    """Future used to monitor a running JSON-RPC request.

    Can be iterated to yield ``JsonRpcProgress`` if the request send any.

    Use ``wait_for_reply`` to wait and get the reply or raise the error.

    Use ``poll`` to refresh the state of the request.

    Use ``cancel`` to send a message to cancel the request.

    Future ready and progress state can also be queried.
    """

    @staticmethod
    def from_reply(reply: JsonRpcReply) -> JsonRpcFuture:
        return JsonRpcFuture(
            task=JsonRpcTask.from_reply(reply),
            cancel=lambda: None,
            poll=lambda _: None,
        )

    def __init__(
        self,
        task: JsonRpcTask,
        cancel: Callable[[], None],
        poll: Callable[[bool], None],
    ) -> None:
        self._task = task
        self._cancel = cancel
        self._poll = poll

    def __iter__(self) -> Iterator[JsonRpcProgress]:
        while True:
            while self.has_progress():
                yield self.get_progress()
            if self.is_ready():
                return
            self.poll(block=True)

    def is_ready(self) -> bool:
        return self._task.is_ready()

    def has_progress(self) -> bool:
        return self._task.has_progress()

    def get_progress(self) -> JsonRpcProgress:
        return self._task.get_progress()

    def wait_for_reply(self) -> JsonRpcReply:
        for _ in self:
            pass
        return self._task.get_reply()

    def poll(self, block: bool) -> None:
        self._poll(block)

    def cancel(self) -> None:
        self._cancel()
