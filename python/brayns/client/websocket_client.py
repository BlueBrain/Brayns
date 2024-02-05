# Copyright 2015-2024 Blue Brain Project/EPFL
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
import ssl
from typing import Callable, Union

import websockets

from .event_loop import EventLoop


class WebsocketClient:

    def __init__(
        self,
        callback: Callable[[Union[bytes, str]], None]
    ) -> None:
        self._callback = callback
        self._loop = EventLoop()
        self._websocket = None
        self._poll_task = None

    @property
    def connected(self) -> bool:
        return self._websocket is not None and self._websocket.open

    def connect(
        self,
        uri: str,
        secure: bool = False,
        cafile: Union[None, str] = None
    ) -> None:
        if self.connected:
            self.disconnect()
        self._loop.start()
        self._loop.run(
            self._connect(uri, secure, cafile)
        ).result()
        self._polling_task = self._loop.run(
            self._poll()
        )

    def disconnect(self) -> None:
        if not self.connected:
            return
        self._polling_task.cancel()
        try:
            self._polling_task.result()
        except concurrent.futures.CancelledError:
            pass
        self._loop.run(
            self._websocket.close()
        ).result()
        self._loop.stop()

    def send(self, data: Union[bytes, str]) -> None:
        self._loop.run(
            self._websocket.send(data)
        ).result()

    async def _connect(self, uri, secure, cafile):
        self._websocket = await websockets.connect(
            uri=('wss://' if secure else 'ws://') + uri,
            ssl=ssl.create_default_context(cafile=cafile) if secure else None,
            ping_interval=None,
            timeout=None,
            max_size=int(2e9)
        )

    async def _poll(self):
        while True:
            self._callback(
                await self._websocket.recv()
            )
