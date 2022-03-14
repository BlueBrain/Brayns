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
from typing import Any, Generator

from .request_context import RequestContext
from ..request_error import RequestError
from ..request_progress import RequestProgress


class RequestTask:

    def __init__(self) -> None:
        self._context = RequestContext()
        self._lock = threading.Condition()

    def set_result(self, result: Any) -> None:
        with self._lock:
            self._context.set_result(result)
            self._lock.notify_all()

    def set_exception(self, error: RequestError) -> None:
        with self._lock:
            self._context.set_exception(error)
            self._lock.notify_all()

    def add_progress(self, progress: RequestProgress) -> None:
        with self._lock:
            self._context.add_progress(progress)
            self._lock.notify_all()

    def wait_for_result(self) -> Any:
        for _ in self.wait_for_all_progresses():
            pass
        return self._context.get_result()

    def wait_for_all_progresses(self) -> Generator[RequestProgress]:
        while True:
            try:
                yield self.wait_for_next_progress()
            except StopIteration:
                return

    def wait_for_next_progress(self) -> RequestProgress:
        with self._lock:
            if self._context.has_progress():
                return self._context.get_progress()
            self._lock.wait()
            return self._context.get_progress()
