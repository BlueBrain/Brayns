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

import threading
from collections import deque

from websockets.client import WebSocketClientProtocol
from websockets.exceptions import ConnectionClosed

from .errors import ConnectionClosedError, ProtocolError


class AsyncWebSocket:
    def __init__(self, websocket: WebSocketClientProtocol) -> None:
        self._websocket = websocket
        self._queue: deque[bytes | str] = deque(maxlen=100)
        self._condition = threading.Condition()
        self._error: Exception | None = None

    @property
    def closed(self) -> bool:
        return self._websocket.closed

    def poll(self, block: bool) -> bytes | str | None:
        with self._condition:
            data = self._get_data()
            if data is not None:
                return data
            if not block:
                return None
            self._condition.wait()
            return self._get_data()

    async def close(self) -> None:
        await self._websocket.close()

    async def send(self, data: bytes | str) -> None:
        try:
            await self._websocket.send(data)
        except ConnectionClosed as e:
            raise ConnectionClosedError(str(e))

    async def run(self) -> None:
        while self._error is None:
            try:
                data = await self._websocket.recv()
            except ConnectionClosed as e:
                self._set_error(ConnectionClosedError(str(e)))
                return
            except Exception as e:
                self._set_error(ProtocolError(str(e)))
                return
            self._set_data(data)

    def _get_data(self) -> bytes | str | None:
        if self._error is not None:
            raise self._error
        if not self._queue:
            return None
        return self._queue.popleft()

    def _set_data(self, data: bytes | str) -> None:
        with self._condition:
            self._queue.append(data)
            self._condition.notify_all()

    def _set_error(self, e: Exception) -> None:
        with self._condition:
            self._error = e
            self._condition.notify_all()
