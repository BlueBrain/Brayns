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

from typing import Any, Optional

from ..request_error import RequestError
from ..request_progress import RequestProgress


class JsonRpcTask:

    @staticmethod
    def from_result(result: Any) -> 'JsonRpcTask':
        task = JsonRpcTask()
        task.set_result(result)
        return task

    def __init__(self) -> None:
        self._ready = False
        self._result: Any = None
        self._error: Optional[RequestError] = None
        self._progress: Optional[RequestProgress] = None

    def is_ready(self) -> None:
        return self._ready

    def has_progress(self) -> None:
        return self._progress is not None

    def get_result(self) -> Any:
        if not self.is_ready():
            raise RuntimeError('Task is still running')
        if self._error is not None:
            raise self._error
        return self._result

    def get_progress(self) -> RequestProgress:
        if self.is_ready():
            raise RuntimeError('Task is not running anymore')
        if not self.has_progress():
            raise RuntimeError('No progresses received yet')
        progress = self._progress
        self._progress = None
        return progress

    def set_result(self, result: Any) -> None:
        self._result = result
        self._ready = True

    def set_error(self, error: RequestError) -> None:
        self._error = error
        self._ready = True

    def add_progress(self, progress: RequestProgress) -> None:
        self._progress = progress
