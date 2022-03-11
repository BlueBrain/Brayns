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

from typing import Any, Union

from .json_rpc_progress import JsonRpcProgress
from .request_error import RequestError
from .request_progress import RequestProgress


class JsonRpcContext:

    def __init__(self) -> None:
        self._ready = False
        self._result: Any = None
        self._exception: Union[RequestError, None] = None
        self._progress: Union[RequestProgress, None] = None

    def is_ready(self) -> None:
        return self._ready

    def has_progress(self) -> None:
        if self.is_ready():
            raise StopIteration()
        return self._progress is not None

    def get_result(self) -> Any:
        if not self.is_ready():
            raise RuntimeError('Task is still running')
        if self._exception is not None:
            raise self._exception
        return self._result

    def get_progress(self) -> JsonRpcProgress:
        if self.is_ready():
            raise StopIteration()
        progress = self._progress
        self._progress = None
        return progress

    def set_result(self, result: Any) -> None:
        self._result = result
        self._ready = True

    def set_exception(self, exception: RequestError) -> None:
        self._exception = exception
        self._ready = True

    def add_progress(self, progress: RequestProgress) -> None:
        self._progress = progress
