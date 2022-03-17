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
import unittest
from typing import Union

from brayns.client.websocket.web_socket import WebSocket

from helpers.web_socket_connection import WebSocketConnection
from helpers.web_socket_server import WebSocketServer


class TestWebSocket(unittest.TestCase):

    def setUp(self) -> None:
        self._uri = 'localhost:5000'
        ssl_folder = pathlib.Path(__file__).parent / 'ssl'
        self._certificate = str(ssl_folder / 'certificate.pem')
        self._key = str(ssl_folder / 'key.pem')
        self._password = 'test'
        self._expected_request = None
        self._expected_reply = None
        self._request = None
        self._reply = None

    def test_text(self) -> None:
        self._send_and_receive(
            request='request',
            reply='reply'
        )

    def test_text_secure(self) -> None:
        self._send_and_receive(
            request='request',
            reply='reply',
            secure=True
        )

    def test_binary(self) -> None:
        self._send_and_receive(
            request=b'request',
            reply=b'reply'
        )

    def test_binary_secure(self) -> None:
        self._send_and_receive(
            request=b'request',
            reply=b'reply',
            secure=True
        )

    def _serve(self, secure: bool = False) -> WebSocketServer:
        return WebSocketServer(
            handle_connection=self._handle_connection,
            uri=self._uri,
            certfile=self._certificate if secure else None,
            keyfile=self._key if secure else None,
            password=self._password if secure else None
        )

    def _connect(self, secure: bool = False) -> WebSocket:
        return WebSocket(
            self._uri,
            secure=secure,
            cafile=self._certificate if secure else None
        )

    def _send_and_receive(
        self,
        request: Union[bytes, str],
        reply: Union[bytes, str],
        secure: bool = False
    ) -> None:
        self._expected_request = request
        self._expected_reply = reply
        with self._serve(secure):
            with self._connect(secure) as client:
                client.send(self._expected_request)
                self._reply = client.receive()
                self.assertEqual(self._request, self._expected_request)
                self.assertEqual(self._reply, self._expected_reply)

    async def _handle_connection(self, connection: WebSocketConnection) -> None:
        self._request = await connection.receive()
        await connection.send(self._expected_reply)


if __name__ == '__main__':
    unittest.main()
