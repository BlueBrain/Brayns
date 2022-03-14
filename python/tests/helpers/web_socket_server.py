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
from typing import Callable, Optional

import websockets
from brayns.client.websocket.event_loop import EventLoop
from brayns.client.websocket.web_socket import WebSocket


class WebSocketServer:

    def __init__(
        self,
        handle_connection: Callable[[WebSocket], None],
        host: str,
        port: int,
        certfile: Optional[str] = None,
        keyfile: Optional[str] = None,
        password: Optional[str] = None
    ) -> None:
        self._handle_connection = handle_connection
        self._loop = EventLoop()
        self._websocket = self._loop.run(
            websockets.serve(
                ws_handler=self._handle,
                host=host,
                port=port,
                ssl=self._create_ssl_context(
                    certfile=certfile,
                    keyfile=keyfile,
                    password=password
                ),
                ping_interval=None,
                timeout=0
            )
        ).result()

    def disconnect(self) -> None:
        self._websocket.close()
        self._loop.run(
            self._websocket.wait_closed()
        ).result()
        self._loop.stop()

    def _create_ssl_context(
        self,
        certfile: Optional[str] = None,
        keyfile: Optional[str] = None,
        password: Optional[str] = None
    ) -> ssl.SSLContext:
        if certfile is None:
            return None
        context = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
        context.load_cert_chain(
            certfile=certfile,
            keyfile=keyfile,
            password=password
        )
        return context

    async def _handle(
        self,
        websocket: websockets.WebSocketClientProtocol
    ) -> None:
        self._handle_connection(WebSocket(websocket, self._loop))
