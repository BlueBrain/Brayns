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

from .json_rpc_error import JsonRpcError
from .json_rpc_progress import JsonRpcProgress
from .json_rpc_reply import JsonRpcReply


class JsonRpcTask:

    def __init__(self) -> None:
        self._reply = None
        self._error = None
        self._progress = None

    def is_ready(self) -> None:
        return self._reply is not None or self._error is not None

    def has_progress(self) -> None:
        if self.is_ready():
            raise StopIteration()
        return self._progress is not None

    def get_reply(self) -> JsonRpcReply:
        if self._error is not None:
            raise self._error
        return self._reply

    def get_progress(self) -> JsonRpcProgress:
        if self.is_ready():
            raise StopIteration()
        progress = self._progress
        self._progress = None
        return progress

    def set_reply(self, reply: JsonRpcReply) -> None:
        self._reply = reply

    def set_error(self, error: JsonRpcError) -> None:
        self._error = error

    def add_progress(self, progress: JsonRpcProgress) -> None:
        self._progress = progress
