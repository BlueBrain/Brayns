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

from typing import Any, Callable, Iterator

from .jsonrpc.json_rpc_task import JsonRpcTask
from .request_progress import RequestProgress


class RequestFuture:
    """Object used to monitor a request."""

    @staticmethod
    def for_notification() -> 'RequestFuture':
        """Create a dummy future for notifications.

        :return: future that returns None immediately
        :rtype: RequestFuture
        """
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
        """Create a future using callbacks and underlying task.

        :param cancel: callback to cancel the request
        :type cancel: Callable[[], None]
        :param receive: callback to poll incoming replies
        :type receive: Callable[[], None]
        :param task: JSON-RPC task bound to the future
        :type task: JsonRpcTask
        """
        self._cancel = cancel
        self._receive = receive
        self._task = task

    def __iter__(self) -> Iterator[RequestProgress]:
        """Yield progress sent by the request.

        :yield: progress messages received during request execution
        :rtype: Iterator[RequestProgress]
        """
        while not self._task.is_ready():
            if self._task.has_progress():
                yield self._task.get_progress()
            self._receive()

    def cancel(self) -> None:
        """Cancel the task if possible."""
        self._cancel()

    def get_result(self) -> Any:
        """Wait for reply and return result from it.

        Raise RequestError if an error message is received.

        :return: result field of the reply
        :rtype: Any
        """
        for _ in self:
            pass
        return self._task.get_result()
