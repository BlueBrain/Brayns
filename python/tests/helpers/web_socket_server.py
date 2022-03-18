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
from typing import Any, Callable, Coroutine, Optional

import websockets
from brayns.client.websocket.event_loop import EventLoop

from .web_socket_connection import WebSocketConnection


class WebSocketServer:

    ConnectionHandler = Callable[
        [WebSocketConnection],
        Coroutine[Any, Any, None]
    ]

    def __init__(
        self,
        handle_connection: ConnectionHandler,
        uri: str,
        certfile: Optional[str] = None,
        keyfile: Optional[str] = None,
        password: Optional[str] = None
    ) -> None:
        self._handle_connection = handle_connection
        host, port = uri.split(':')
        self._host = host
        self._port = int(port)
        self._ssl = None
        if certfile is not None:
            self._ssl = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
            self._ssl.load_cert_chain(
                certfile=certfile,
                keyfile=keyfile,
                password=password
            )
        self._loop = EventLoop()
        self._loop.run(
            self._start()
        ).result()

    def __enter__(self) -> 'WebSocketServer':
        return self

    def __exit__(self, *_) -> None:
        self.stop()

    def stop(self) -> None:
        self._websocket.close()
        self._loop.run(
            self._websocket.wait_closed()
        ).result()
        self._loop.close()

    async def _start(self) -> None:
        self._websocket = await websockets.serve(
            ws_handler=self._handle,
            host=self._host,
            port=self._port,
            ssl=self._ssl,
            ping_interval=None,
            close_timeout=0
        )

    async def _handle(
        self,
        websocket: websockets.WebSocketServerProtocol,
        *_
    ) -> None:
        await self._handle_connection(
            WebSocketConnection(websocket)
        )
