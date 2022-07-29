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

from typing import Optional

from brayns.instance.websocket.async_web_socket_connector import AsyncWebSocketConnector
from brayns.instance.websocket.event_loop import EventLoop
from brayns.instance.websocket.ssl_client_context import SslClientContext
from brayns.instance.websocket.web_socket_client import WebSocketClient
from brayns.instance.websocket.web_socket_listener import WebSocketListener


class WebSocketConnector:

    def __init__(
        self,
        uri: str,
        listener: WebSocketListener,
        ssl: Optional[SslClientContext] = None
    ) -> None:
        self._connector = AsyncWebSocketConnector(uri, ssl)
        self._listener = listener
        self._loop = EventLoop()

    def connect(self) -> WebSocketClient:
        websocket = self._loop.run(
            self._connector.connect()
        ).result()
        return WebSocketClient(
            websocket=websocket,
            loop=self._loop,
            listener=self._listener
        )
