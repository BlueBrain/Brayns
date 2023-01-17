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

from typing import Any

import brayns
from brayns.network.jsonrpc import JsonRpcListener


class MockJsonRpcListener(JsonRpcListener):
    def __init__(self) -> None:
        self._called = False
        self._data = None

    def get_data(self) -> Any:
        if not self._called:
            raise RuntimeError("Data not received")
        return self._data

    def on_reply(self, reply: brayns.JsonRpcReply) -> None:
        self._set_data(reply)

    def on_error(self, error: brayns.JsonRpcError) -> None:
        self._set_data(error)

    def on_progress(self, progress: brayns.JsonRpcProgress) -> None:
        self._set_data(progress)

    def on_invalid_message(self, e: Exception) -> None:
        self._set_data(e)

    def _set_data(self, data: Any) -> None:
        if self._called:
            raise RuntimeError("Data received twice")
        self._called = True
        self._data = data
