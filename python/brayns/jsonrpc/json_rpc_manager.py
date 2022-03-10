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

from .json_rpc_client import JsonRpcClient
from .json_rpc_error import JsonRpcError
from .json_rpc_progress import JsonRpcProgress
from .json_rpc_reply import JsonRpcReply
from .json_rpc_request import JsonRpcRequest
from .json_rpc_future import JsonRpcFuture


class JsonRpcManager:

    def __init__(self, client: JsonRpcClient) -> None:
        self._client = client
        self._futures = {}
        self._client.start(self)

    def send(self, request: JsonRpcRequest) -> JsonRpcFuture:
        future = JsonRpcFuture()
        self._futures[request.id] = future
        return future

    def on_reply(self, reply: JsonRpcReply) -> None:
        future = self._futures.get(reply.id)
        if future is None:
            return
        future.set_reply(reply)

    def on_error(self, error: JsonRpcError) -> None:
        future = self._futures.get(error.id)
        if future is None:
            return
        future.set_error(error)

    def on_progress(self, progress: JsonRpcProgress) -> None:
        future = self._futures.get(progress.id)
        if future is None:
            return
        future.add_progress(progress)

    def on_invalid_message(self, data: str, e: Exception) -> None:
        print(f'Invalid message received: {data} {e}')
