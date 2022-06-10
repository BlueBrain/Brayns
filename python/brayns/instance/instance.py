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

from typing import Any, Optional, Protocol, TypeVar

from brayns.instance.jsonrpc.json_rpc_id import JsonRpcId
from brayns.instance.jsonrpc.json_rpc_request import JsonRpcRequest
from brayns.instance.request_future import RequestFuture

T = TypeVar('T', bound='Instance')


class Instance(Protocol):

    def __enter__(self: T) -> T:
        return self

    def __exit__(self, *_) -> None:
        self.disconnect()

    def disconnect(self) -> None:
        pass

    def request(self, method: str, params: Any = None) -> Any:
        task = self.task(method, params)
        return task.wait_for_result()

    def task(self, method: str, params: Any = None) -> RequestFuture:
        id = 0
        while self.is_running(id):
            id += 1
        request = JsonRpcRequest(id, method, params)
        return self.send(request)

    def is_running(self, id: JsonRpcId) -> bool:
        return False

    def send(self, request: JsonRpcRequest) -> RequestFuture:
        raise NotImplementedError()

    def poll(self, block: bool = True, timeout: Optional[float] = None) -> None:
        pass

    def cancel(self, id: JsonRpcId) -> None:
        pass
