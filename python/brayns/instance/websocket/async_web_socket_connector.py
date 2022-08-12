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

import ssl
from dataclasses import dataclass

from websockets.client import connect
from websockets.exceptions import WebSocketException

from .async_web_socket import AsyncWebSocket
from .invalid_server_certificate_error import InvalidServerCertificateError
from .protocol_error import ProtocolError
from .service_unavailable_error import ServiceUnavailableError
from .ssl_client_context import SslClientContext


@dataclass
class AsyncWebSocketConnector:

    uri: str
    ssl_context: SslClientContext | None = None

    async def connect(self) -> AsyncWebSocket:
        try:
            websocket = await connect(
                uri=self._format_uri(),
                ssl=self._create_ssl_context(),
                ping_interval=None,
                close_timeout=0,
                max_size=int(2e9)
            )
            return AsyncWebSocket(websocket)
        except ConnectionRefusedError as e:
            raise ServiceUnavailableError(str(e))
        except ssl.SSLError as e:
            raise InvalidServerCertificateError(str(e))
        except WebSocketException as e:
            raise ProtocolError(str(e))

    def _format_uri(self) -> str:
        protocol = 'ws://' if self.ssl_context is None else 'wss://'
        return protocol + self.uri

    def _create_ssl_context(self) -> ssl.SSLContext | None:
        if self.ssl_context is None:
            return None
        return self.ssl_context.create()
