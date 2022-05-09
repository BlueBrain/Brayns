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
from typing import Optional

import websockets
from brayns.instance.websocket.event_loop import EventLoop


class EchoServer:

    @staticmethod
    def start(
        uri: str,
        certfile: Optional[str] = None,
        keyfile: Optional[str] = None,
        password: Optional[str] = None
    ) -> 'EchoServer':
        loop = EventLoop()
        return EchoServer(
            websocket=loop.run(
                EchoServer._start(uri, certfile, keyfile, password)
            ).result(),
            loop=loop
        )

    @staticmethod
    async def _echo(websocket: websockets.WebSocketServerProtocol, _) -> None:
        try:
            data = await websocket.recv()
            await websocket.send(data)
        except Exception:
            pass

    @staticmethod
    async def _start(
        uri: str,
        certfile: Optional[str] = None,
        keyfile: Optional[str] = None,
        password: Optional[str] = None
    ) -> websockets.WebSocketServer:
        host, port = uri.split(':')
        context = None
        if certfile is not None:
            context = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
            context.load_cert_chain(certfile, keyfile, password)
        return await websockets.serve(
            ws_handler=EchoServer._echo,
            host=host,
            port=int(port),
            ssl=context,
            ping_interval=None,
            close_timeout=0
        )

    def __init__(
        self,
        websocket: websockets.WebSocketServer,
        loop: EventLoop
    ) -> None:
        self._websocket = websocket
        self._loop = loop

    def __enter__(self) -> 'EchoServer':
        return self

    def __exit__(self, *_) -> None:
        self.stop()

    def stop(self) -> None:
        self._websocket.close()
        self._loop.run(
            self._websocket.wait_closed()
        ).result()
        self._loop.close()
