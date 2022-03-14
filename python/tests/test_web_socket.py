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

import unittest
from typing import Union

from brayns.client.websocket.web_socket_protocol import WebSocketProtocol

from helpers.client_and_server import ClientAndServer


class TestWebSocket(unittest.TestCase):

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

    def _send_and_receive(
        self,
        request: Union[bytes, str],
        reply: Union[bytes, str],
        secure: bool = False
    ) -> None:
        self._expected_request = request
        self._expected_reply = reply
        self._request = None
        self._reply = None
        with ClientAndServer(self._handle, secure) as client_and_server:
            client = client_and_server.client
            client.send(self._expected_request)
            self._reply = client.receive()
            self.assertEqual(self._request, self._expected_request)
            self.assertEqual(self._reply, self._expected_reply)

    def _handle(self, websocket: WebSocketProtocol) -> None:
        self._request = websocket.receive()
        websocket.send(self._expected_reply)


if __name__ == '__main__':
    unittest.main()
