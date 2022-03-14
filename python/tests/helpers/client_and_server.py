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

import pathlib
from typing import Callable

from brayns.client.websocket.web_socket import WebSocket

from helpers.web_socket_server import WebSocketServer

HOST = 'localhost'
PORT = 5000
URI = f'{HOST}:{PORT}'
SSL_FOLDER = pathlib.Path(__file__).parent.parent / 'ssl'
CERTIFICATE = str(SSL_FOLDER / 'certificate.pem')
KEY = str(SSL_FOLDER / 'key.pem')
PASSWORD = 'test'


class ClientAndServer:

    def __init__(
        self,
        handle_connection: Callable[[WebSocket], None],
        secure: bool = False
    ) -> None:
        self._server = WebSocketServer(
            handle_connection=handle_connection,
            host=HOST,
            port=PORT,
            certfile=CERTIFICATE if secure else None,
            keyfile=KEY if secure else None,
            password=PASSWORD if secure else None
        )
        self._client = WebSocket.connect(
            uri=URI,
            secure=secure,
            cafile=CERTIFICATE if secure else None
        )

    def __enter__(self) -> 'ClientAndServer':
        return self

    def __exit__(self, *_) -> None:
        self._client.disconnect()
        self._server.disconnect()

    @property
    def client(self) -> 'WebSocket':
        return self._client
