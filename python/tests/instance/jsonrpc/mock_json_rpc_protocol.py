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

from brayns.instance.jsonrpc.json_rpc_error import JsonRpcError
from brayns.instance.jsonrpc.json_rpc_progress import JsonRpcProgress
from brayns.instance.jsonrpc.json_rpc_reply import JsonRpcReply


class MockJsonRpcProtocol:

    def __init__(self) -> None:
        self._called = False
        self._data = None

    def get_data(self) -> Any:
        assert self._called
        return self._data

    def on_binary(self, data: bytes) -> None:
        self._set_data(data)

    def on_reply(self, reply: JsonRpcReply) -> None:
        self._set_data(reply)

    def on_error(self, error: JsonRpcError) -> None:
        self._set_data(error)

    def on_progress(self, progress: JsonRpcProgress) -> None:
        self._set_data(progress)

    def on_invalid_frame(self, data: Union[bytes, str], e: Exception) -> None:
        self._set_data((data, e))

    def _set_data(self, data: Any) -> None:
        assert not self._called
        self._called = True
        self._data = data
