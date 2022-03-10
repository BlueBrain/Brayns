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

import threading
from typing import Generator

from .json_rpc_error import JsonRpcError
from .json_rpc_progress import JsonRpcProgress
from .json_rpc_reply import JsonRpcReply
from .json_rpc_task import JsonRpcTask


class JsonRpcFuture:

    def __init__(self) -> None:
        self._task = JsonRpcTask()
        self._lock = threading.Condition()

    def set_reply(self, reply: JsonRpcReply) -> None:
        with self._lock:
            self._task.set_reply(reply)
            self._lock.notify_all()

    def set_error(self, error: JsonRpcError) -> None:
        with self._lock:
            self._task.set_error(error)
            self._lock.notify_all()

    def add_progress(self, progress: JsonRpcProgress) -> None:
        with self._lock:
            self._task.add_progress(progress)
            self._lock.notify_all()

    def wait_for_reply(self) -> JsonRpcReply:
        for progress in self.wait_for_all_progresses():
            pass
        return self._task.get_reply()

    def wait_for_all_progresses(self) -> Generator[JsonRpcProgress]:
        while True:
            try:
                yield self._wait_for_next_progress()
            except StopIteration:
                return

    def _wait_for_next_progress(self) -> JsonRpcProgress:
        with self._lock:
            if self._task.has_progress():
                return self._task.get_progress()
            self._lock.wait()
            return self._task.get_progress()
