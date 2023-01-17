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
from typing import Generic, TypeVar

from .jsonrpc import JsonRpcFuture, JsonRpcProgress, JsonRpcReply

T = TypeVar("T")


class Future(Generic[T]):
    """JSON-RPC future wrapper to include the reply processing.

    Add a new method ``wait_for_result`` to retreive the result of the reply
    processing.
    """

    def __init__(
        self, future: JsonRpcFuture, reply_handler: Callable[[JsonRpcReply], T]
    ) -> None:
        self._future = future
        self._reply_handler = reply_handler

    def __iter__(self) -> Iterator[JsonRpcProgress]:
        yield from self._future

    def is_ready(self) -> bool:
        return self._future.is_ready()

    def has_progress(self) -> bool:
        return self._future.has_progress()

    def get_progress(self) -> JsonRpcProgress:
        return self._future.get_progress()

    def wait_for_reply(self) -> JsonRpcReply:
        return self._future.wait_for_reply()

    def wait_for_result(self) -> T:
        reply = self.wait_for_reply()
        return self._reply_handler(reply)

    def poll(self, block: bool) -> None:
        self._future.poll(block)

    def cancel(self) -> None:
        self._future.cancel()
