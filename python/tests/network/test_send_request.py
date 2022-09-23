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

import json
import unittest

import brayns
from brayns.network import send_request, serialize_request

from .mock_listener import MockListener
from .mock_web_socket import MockWebSocket


class TestSendRequest(unittest.TestCase):

    def test_send_request_binary(self) -> None:
        ref_binary = b'123'
        request = self._create_request(ref_binary)
        ref_text = self._serialize_json(request)
        websocket = self._send(request)
        self.assertFalse(websocket.text_request)
        test = websocket.binary_request
        size = int.from_bytes(test[:4], byteorder='little', signed=False)
        self.assertEqual(size, len(ref_text))
        text = test[4:size+4].decode('utf-8')
        self.assertEqual(text, ref_text)
        binary = test[size+4:]
        self.assertEqual(binary, ref_binary)

    def test_send_request_text(self) -> None:
        request = self._create_request()
        ref = self._serialize_json(request)
        websocket = self._send(request)
        self.assertFalse(websocket.binary_request)
        test = websocket.text_request
        self.assertEqual(test, ref)

    def _create_request(self, binary: bytes = b'') -> brayns.Request:
        return brayns.Request(
            id=0,
            method='test',
            params=123,
            binary=binary,
        )

    def _serialize_json(self, request: brayns.Request) -> str:
        message = serialize_request(request)
        return json.dumps(message)

    def _send(self, request: brayns.Request) -> MockWebSocket:
        listener = MockListener()
        websocket = MockWebSocket(listener)
        send_request(request, websocket)
        return websocket


if __name__ == '__main__':
    unittest.main()
