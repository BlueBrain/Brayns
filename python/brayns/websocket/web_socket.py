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

import ssl
from typing import Union

import websockets

from .web_socket_listener import WebSocketListener


class WebSocket:

    def __init__(
        self,
        uri: str,
        secure: bool = False,
        cafile: Union[str, None] = None
    ) -> None:
        self._uri = ('wss://' if secure else 'ws://') + uri
        self._ssl = ssl.create_default_context(
            cafile=cafile
        ) if secure else None
        self._websocket = None

    async def connect(self) -> None:
        self._websocket = await websockets.connect(
            self._uri,
            ssl=self._ssl,
            ping_interval=None,
            timeout=None,
            max_size=int(2e9)
        )

    async def disconnect(self) -> None:
        self._websocket.close()

    async def send_binary(self, data: bytes) -> None:
        await self._websocket.send(data)

    async def send_text(self, data: str) -> None:
        await self._websocket.send(data)

    async def poll(self, listener: WebSocketListener):
        async for data in self._websocket:
            if isinstance(data, bytes):
                listener.on_binary_frame(data)
            if isinstance(data, str):
                listener.on_text_frame(data)
            raise RuntimeError('Invalid data received')
