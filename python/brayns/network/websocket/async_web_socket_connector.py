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

import asyncio
import ssl
from dataclasses import dataclass

from websockets.client import connect
from websockets.exceptions import WebSocketException

from .async_web_socket import AsyncWebSocket
from .exceptions import (
    InvalidServerCertificateError,
    ProtocolError,
    ServiceUnavailableError,
)
from .ssl_client_context import SslClientContext


@dataclass
class AsyncWebSocketConnector:

    uri: str
    ssl_context: SslClientContext | None = None

    async def connect(self) -> AsyncWebSocket:
        try:
            websocket = await connect(
                uri=_format_uri(self.uri, self.ssl_context),
                ssl=_try_create_ssl_context(self.ssl_context),
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
    protocol = 'ws://' if context is None else 'wss://'
    return protocol + uri


def _try_create_ssl_context(context: SslClientContext | None) -> ssl.SSLContext | None:
    if context is None:
        return None
    return _create_ssl_context(context)


def _create_ssl_context(context: SslClientContext) -> ssl.SSLContext:
    return ssl.create_default_context(
        cafile=context.cafile,
        capath=context.capath,
        cadata=context.cadata
    )
