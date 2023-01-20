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

from collections import deque

from .json_rpc_error import JsonRpcError
from .json_rpc_progress import JsonRpcProgress
from .json_rpc_reply import JsonRpcReply


class JsonRpcTask:
    @staticmethod
    def from_reply(reply: JsonRpcReply) -> JsonRpcTask:
        task = JsonRpcTask()
        task.set_reply(reply)
        return task

    def __init__(self) -> None:
        self._reply: JsonRpcReply | None = None
        self._error: JsonRpcError | None = None
        self._progresses = deque[JsonRpcProgress]()

    def is_ready(self) -> bool:
        return self._reply is not None or self._error is not None

    def has_progress(self) -> bool:
        return bool(self._progresses)

    def get_reply(self) -> JsonRpcReply:
        if self._error is not None:
            raise self._error
        if self._reply is None:
            raise RuntimeError("Task is still running")
        return self._reply

    def set_reply(self, reply: JsonRpcReply) -> None:
        if self.is_ready():
            raise RuntimeError("Task already finished")
        self._reply = reply

    def set_error(self, error: JsonRpcError) -> None:
        if self.is_ready():
            raise RuntimeError("Task already finished")
        self._error = error

    def get_progress(self) -> JsonRpcProgress:
        if not self._progresses:
            raise RuntimeError("No progress received")
        return self._progresses.popleft()

    def add_progress(self, progress: JsonRpcProgress) -> None:
        if self.is_ready():
            raise RuntimeError("Task already finished")
        self._progresses.append(progress)
