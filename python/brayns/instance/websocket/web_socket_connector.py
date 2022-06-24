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

from brayns.instance.websocket.async_web_socket import AsyncWebSocket
from brayns.instance.websocket.event_loop import EventLoop
from brayns.instance.websocket.web_socket_client import WebSocketClient
from brayns.instance.websocket.web_socket_listener import WebSocketListener


class WebSocketConnector:

    def __init__(
        self,
        uri: str,
        listener: WebSocketListener,
        secure: bool = False,
        cafile: Optional[str] = None
    ) -> None:
        self._uri = ('wss://' if secure else 'ws://') + uri
        self._listener = listener
        self._ssl = None
        if secure:
            self._ssl = ssl.create_default_context(cafile=cafile)
        self._loop = EventLoop()

    def connect(self) -> WebSocketClient:
        websocket = self._loop.run(
            AsyncWebSocket.connect(self._uri, self._ssl)
        ).result()
        task = self._loop.run(
            websocket.run()
        )
        return WebSocketClient(
            websocket=websocket,
            loop=self._loop,
            task=task,
            listener=self._listener
        )
