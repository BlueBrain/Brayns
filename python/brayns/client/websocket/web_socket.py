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

from .event_loop import EventLoop


class WebSocket:

    @staticmethod
    def connect(
        uri: str,
        secure: bool = False,
        cafile: Optional[str] = None
    ) -> 'WebSocket':
        loop = EventLoop()
        return WebSocket(
            loop.run(
                websockets.connect(
                    uri=('wss://' if secure else 'ws://') + uri,
                    ssl=ssl.create_default_context(
                        cafile=cafile
                    ) if secure else None,
                    ping_interval=None,
                    timeout=None,
                    max_size=int(2e9)
                )
            ).result(),
            loop
        )

    def __init__(
        self,
        websocket: websockets.WebSocketClientProtocol,
        loop: EventLoop
    ) -> None:
        self._websocket = websocket
        self._loop = loop

    def disconnect(self) -> None:
        self._loop.run(
            self._websocket.close()
        ).result()

    def receive(self) -> Union[bytes, str]:
        return self._loop.run(
            self._websocket.recv()
        ).result()

    def send(self, data: Union[bytes, str]) -> None:
        self._loop.run(
            self._websocket.send(data)
        ).result()
