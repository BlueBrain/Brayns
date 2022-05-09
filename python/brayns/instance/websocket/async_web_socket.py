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
from typing import Optional, Union

import websockets
from brayns.instance.websocket.web_socket_error import WebSocketError


class AsyncWebSocket:

    @staticmethod
    async def connect(uri: str, ssl: Optional[ssl.SSLContext]) -> 'AsyncWebSocket':
        try:
            websocket = await websockets.connect(
                uri=uri,
                ssl=ssl,
                ping_interval=None,
                close_timeout=0,
                max_size=int(2e9)
            )
            return AsyncWebSocket(websocket)
        except Exception as e:
            raise WebSocketError(str(e))

    def __init__(self,  websocket: websockets.WebSocketClientProtocol) -> None:
        self._websocket = websocket

    @property
    def closed(self) -> bool:
        return self._websocket.closed

    async def close(self) -> None:
        try:
            await self._websocket.close()
        except Exception as e:
            raise WebSocketError(str(e))

    async def receive(self) -> Union[bytes, str]:
        try:
            return await self._websocket.recv()
        except Exception as e:
            raise WebSocketError(str(e))

    async def send(self, data: Union[bytes, str]) -> None:
        try:
            await self._websocket.send(data)
        except Exception as e:
            raise WebSocketError(str(e))
