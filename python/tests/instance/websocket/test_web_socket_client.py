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
from typing import Optional, Union

from brayns.instance.websocket.web_socket_client import WebSocketClient
from brayns.instance.websocket.web_socket_error import WebSocketError
from tests.instance.websocket.mock_listener import MockListener
from tests.instance.websocket.mock_server import MockServer


class TestWebSocketClient(unittest.TestCase):

    def setUp(self) -> None:
        self._uri = 'localhost:5000'
        ssl_folder = pathlib.Path(__file__).parent / 'ssl'
        self._certificate = str(ssl_folder / 'certificate.pem')
        self._key = str(ssl_folder / 'key.pem')
        self._password = 'test'
        self._listener = MockListener()

    def test_connect_error(self) -> None:
        with self.assertRaises(WebSocketError):
            self._connect()

    def test_connect(self) -> None:
        with self._start_server(receive=True):
            with self._connect():
                pass

    def test_closed(self) -> None:
        with self._start_server(receive=True):
            with self._connect() as websocket:
                self.assertFalse(websocket.closed)
        self.assertTrue(websocket.closed)

    def test_close(self) -> None:
        with self._start_server(receive=True):
            websocket = self._connect()
            websocket.close()
            self.assertTrue(websocket.closed)

    def test_poll(self) -> None:
        with self._start_server(reply='test'):
            with self._connect() as websocket:
                websocket.poll()
                self.assertEqual(self._listener.text, 'test')

    def test_send_binary(self) -> None:
        with self._start_server(receive=True) as server:
            with self._connect() as websocket:
                websocket.send_binary(b'test')
                request = server.wait_for_request()
                self.assertEqual(request, b'test')

    def test_send_text(self) -> None:
        with self._start_server(receive=True) as server:
            with self._connect() as websocket:
                websocket.send_text('test')
                request = server.wait_for_request()
                self.assertEqual(request, 'test')

    def test_send_text_secure(self) -> None:
        with self._start_server(
            receive=True,
            reply='test1',
            secure=True
        ) as server:
            with self._connect(secure=True) as websocket:
                websocket.send_text('test2')
                request = server.wait_for_request()
                self.assertEqual(request, 'test2')
                websocket.poll()
                self.assertEqual(self._listener.text, 'test1')

    def _start_server(
        self,
        receive: bool = False,
        reply: Optional[Union[bytes, str]] = None,
        secure: bool = False
    ) -> MockServer:
        return MockServer(
            uri=self._uri,
            receive=receive,
            reply=reply,
            certfile=self._certificate if secure else None,
            keyfile=self._key if secure else None,
            password=self._password if secure else None
        )

    def _connect(self, secure: bool = False) -> WebSocketClient:
        return WebSocketClient.connect(
            self._uri,
            self._listener,
            secure=secure,
            cafile=self._certificate if secure else None
        )


if __name__ == '__main__':
    unittest.main()
