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

import json_rpc_handler
from .json_rpc_request import JsonRpcRequest
from .json_rpc_listener import JsonRpcListener
from ..websocket.web_socket_client import WebSocketClient


class JsonRpcClient:

    def __init__(self, client: WebSocketClient) -> None:
        self._client = client
        self._listener = None

    def start(self, listener: JsonRpcListener) -> None:
        self._listener = listener
        self._client.start(self)

    def stop(self) -> None:
        self._client.stop()

    def send(self, request: JsonRpcRequest) -> None:
        self._client.send_text(request.to_json())

    def on_binary_frame(self, data: bytes) -> None:
        pass

    def on_text_frame(self, data: str) -> None:
        json_rpc_handler.handle_text(data, self._listener)
