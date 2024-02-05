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

import ssl
from typing import Callable, Union

import websockets

from brayns.client.event_loop import EventLoop


class MockServer:

    RequestHandler = Callable[[Union[bytes, str]], Union[bytes, str, None]]

    def __init__(
        self,
        host: str,
        port: int,
        request_handler: RequestHandler,
        secure: bool = False,
        certfile: str = None,
        keyfile: str = None,
        password: str = None
    ) -> None:
        self._host = host
        self._port = port
        self._request_handler = request_handler
        self._ssl = self._get_ssl(
            certfile=certfile,
            keyfile=keyfile,
            password=password
        ) if secure else None
        self._websocket = None
        self._loop = EventLoop()

    def start(self) -> None:
        self._loop.start()
        self._loop.run(self._start()).result()

    def stop(self) -> None:
        self._websocket.close()
        self._loop.run(self._websocket.wait_closed()).result()
        self._loop.stop()

    def _get_ssl(self, certfile, keyfile, password):
        context = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
        context.load_cert_chain(
            certfile=certfile,
            keyfile=keyfile,
            password=password
        )
        return context

    async def _handle_connection(
        self,
        websocket: websockets.WebSocketServerProtocol,
        path: str
    ) -> None:
        try:
            await self._process_requests(websocket)
        except websockets.ConnectionClosed:
            pass

    async def _process_requests(
        self,
        websocket: websockets.WebSocketServerProtocol
    ) -> None:
        while websocket.open:
            request = await websocket.recv()
            reply = self._request_handler(request)
            if reply is not None:
                await websocket.send(reply)

    async def _start(self) -> None:
        self._websocket = await websockets.serve(
            self._handle_connection,
            self._host,
            self._port,
            ssl=self._ssl,
            ping_interval=None,
            timeout=0
        )
