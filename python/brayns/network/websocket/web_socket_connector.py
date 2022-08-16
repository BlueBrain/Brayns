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

from __future__ import annotations

from dataclasses import dataclass

from .async_web_socket_connector import AsyncWebSocketConnector
from .event_loop import EventLoop
from .ssl_client_context import SslClientContext
from .web_socket_client import WebSocketClient
from .web_socket_listener import WebSocketListener


@dataclass
class WebSocketConnector:

    uri: str
    listener: WebSocketListener
    ssl_context: SslClientContext | None = None

    def connect(self) -> WebSocketClient:
        loop = EventLoop()
        connector = AsyncWebSocketConnector(self.uri, self.ssl_context)
        try:
            websocket = loop.run(
                connector.connect()
            ).result()
        except:
            loop.close()
            raise
        return WebSocketClient(websocket, loop, self.listener)
