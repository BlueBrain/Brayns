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

import asyncio
import ssl
from dataclasses import dataclass

from websockets.client import connect
from websockets.exceptions import WebSocketException

from .async_web_socket import AsyncWebSocket
from .errors import (
    InvalidServerCertificateError,
    ProtocolError,
    ServiceUnavailableError,
)
from .event_loop import EventLoop
from .web_socket_client import WebSocketClient
from .web_socket_listener import WebSocketListener


@dataclass
class SslClientContext:

    cafile: str | None = None
    capath: str | None = None
    cadata: str | None = None


@dataclass
class WebSocketConnector:

    uri: str
    listener: WebSocketListener
    ssl_context: SslClientContext | None = None

    def connect(self) -> WebSocketClient:
        loop = EventLoop()
        try:
            websocket = loop.run(_connect(self.uri, self.ssl_context)).result()
        except BaseException:
            loop.close()
            raise
        return WebSocketClient(websocket, loop, self.listener)


async def _connect(uri: str, context: SslClientContext | None) -> AsyncWebSocket:
    try:
        websocket = await connect(
            uri=_format_uri(uri, context),
            ssl=_try_create_ssl_context(context),
            ping_interval=None,
            close_timeout=0,
            max_size=int(2e9),
        )
        return AsyncWebSocket(websocket)
    except WebSocketException as e:
        raise ProtocolError(str(e))
    except ssl.SSLError as e:
        raise InvalidServerCertificateError(str(e))
    except OSError as e:
        raise ServiceUnavailableError(str(e))
    except asyncio.TimeoutError as e:
        raise ServiceUnavailableError(str(e))


def _format_uri(uri: str, context: SslClientContext | None) -> str:
    protocol = "ws://" if context is None else "wss://"
    return protocol + uri


def _try_create_ssl_context(context: SslClientContext | None) -> ssl.SSLContext | None:
    if context is None:
        return None
    return ssl.create_default_context(
        cafile=context.cafile, capath=context.capath, cadata=context.cadata
    )
