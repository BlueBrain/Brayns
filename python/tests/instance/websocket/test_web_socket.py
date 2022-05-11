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

from brayns.instance.websocket.web_socket_client import WebSocketClient
from brayns.instance.websocket.web_socket_error import WebSocketError
from tests.instance.websocket.echo_server import EchoServer


class TestWebSocket(unittest.TestCase):

    def setUp(self) -> None:
        self._uri = 'localhost:5000'
        ssl_folder = pathlib.Path(__file__).parent / 'ssl'
        self._certificate = str(ssl_folder / 'certificate.pem')
        self._key = str(ssl_folder / 'key.pem')
        self._password = 'test'

    def test_connect_error(self) -> None:
        with self.assertRaises(WebSocketError):
            self._connect()

    def test_connect(self) -> None:
        with self._start_echo_server():
            with self._connect():
                pass

    def test_closed(self) -> None:
        with self._start_echo_server():
            with self._connect() as websocket:
                self.assertFalse(websocket.closed)
        self.assertTrue(websocket.closed)

    def test_close(self) -> None:
        with self._start_echo_server():
            websocket = self._connect()
            websocket.close()
            self.assertTrue(websocket.closed)

    def test_send_receive_text(self) -> None:
        self._send_and_receive('test')

    def test_send_receive_text_secure(self) -> None:
        self._send_and_receive('test', secure=True)

    def test_send_receive_binary(self) -> None:
        self._send_and_receive(b'test')

    def test_send_receive_binary_secure(self) -> None:
        self._send_and_receive(b'test', secure=True)

    def _start_echo_server(self, secure: bool = False) -> EchoServer:
        return EchoServer.start(
            uri=self._uri,
            certfile=self._certificate if secure else None,
            keyfile=self._key if secure else None,
            password=self._password if secure else None
        )

    def _connect(self, secure: bool = False) -> WebSocketClient:
        return WebSocketClient.connect(
            self._uri,
            secure=secure,
            cafile=self._certificate if secure else None
        )

    def _send_and_receive(
        self,
        request: Union[bytes, str],
        secure: bool = False
    ) -> None:
        with self._start_echo_server(secure):
            with self._connect(secure) as websocket:
                websocket.send(request)
                reply = websocket.receive()
                self.assertEqual(reply, request)


if __name__ == '__main__':
    unittest.main()
