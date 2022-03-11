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

from typing import Any
import websockets

from .web_socket_listener import WebSocketListener


class WebSocket:

    def __init__(self, websocket: websockets.WebSocketCommonProtocol) -> None:
        self._websocket = websocket

    async def disconnect(self) -> None:
        await self._websocket.close()

    async def send_binary(self, data: bytes) -> None:
        await self._websocket.send(data)

    async def send_text(self, data: str) -> None:
        await self._websocket.send(data)

    async def receive(self, listener: WebSocketListener) -> None:
        data = self._websocket.recv()
        await self._dispatch(data, listener)

    async def poll(self, listener: WebSocketListener) -> None:
        async for data in self._websocket:
            await self._dispatch(data, listener)

    async def _dispatch(self, data: Any, listener: WebSocketListener) -> None:
        if isinstance(data, bytes):
            listener.on_binary_frame(data)
        if isinstance(data, str):
            listener.on_text_frame(data)
        raise RuntimeError('Invalid data received')
