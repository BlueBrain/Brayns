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

from __future__ import annotations

from .async_web_socket import AsyncWebSocket
from .errors import ConnectionClosedError
from .event_loop import EventLoop
from .web_socket import WebSocket
from .web_socket_listener import WebSocketListener


class WebSocketClient(WebSocket):
    def __init__(
        self, websocket: AsyncWebSocket, loop: EventLoop, listener: WebSocketListener
    ) -> None:
        self._websocket = websocket
        self._loop = loop
        self._listener = listener
        self._task = self._loop.run(self._websocket.run())

    @property
    def closed(self) -> bool:
        return self._websocket.closed and self._loop.closed

    def close(self) -> None:
        if self.closed:
            return
        self._loop.run(self._websocket.close()).result()
        self._task.result()
        self._loop.close()

    def poll(self, block: bool) -> None:
        self._check_closed()
        data = self._websocket.poll(block)
        if data is None:
            return
        if isinstance(data, bytes):
            self._listener.on_binary(data)
            return
        if isinstance(data, str):
            self._listener.on_text(data)
            return
        raise TypeError(f"Invalid packet type {type(data)}")

    def send_binary(self, data: bytes) -> None:
        self._send(data)

    def send_text(self, data: str) -> None:
        self._send(data)

    def _send(self, data: bytes | str) -> None:
        self._check_closed()
        self._loop.run(self._websocket.send(data)).result()

    def _check_closed(self) -> None:
        if self.closed:
            raise ConnectionClosedError("Connection closed")
