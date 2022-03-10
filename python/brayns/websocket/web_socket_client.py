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

import concurrent.futures

from .event_loop import EventLoop
from .web_socket import WebSocket
from .web_socket_listener import WebSocketListener


class WebSocketClient:

    def __init__(self, websocket: WebSocket) -> None:
        self._websocket = websocket
        self._listener = None
        self._loop = None
        self._task = None

    def start(self, listener: WebSocketListener) -> None:
        self._listener = listener
        self._loop = EventLoop()
        self._loop.run(
            self._websocket.connect()
        ).result()
        self._task = self._loop.run(
            self._websocket.poll(listener)
        )

    def stop(self) -> None:
        self._task.cancel()
        try:
            self._task.result()
        except concurrent.futures.CancelledError:
            pass
        self._loop.run(
            self._websocket.disconnect()
        ).result()
        self._loop.stop()

    def send_binary(self, data: bytes) -> None:
        self._loop.run(
            self._websocket.send_binary(data)
        ).result()

    def send_text(self, data: str) -> None:
        self._loop.run(
            self._websocket.send_text(data)
        ).result()
