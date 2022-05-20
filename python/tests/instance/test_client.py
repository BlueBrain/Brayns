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
import logging
import unittest

from brayns.instance.client import Client
from brayns.instance.jsonrpc.json_rpc_request import JsonRpcRequest
from tests.instance.websocket.mock_web_socket import MockWebSocket


class TestClient(unittest.TestCase):

    def setUp(self) -> None:
        self._websocket = MockWebSocket()

    def test_context(self) -> None:
        with self._connect():
            self.assertFalse(self._websocket.closed)
        self.assertTrue(self._websocket.closed)

    def test_disconnect(self) -> None:
        client = self._connect()
        self.assertFalse(self._websocket.closed)
        client.disconnect()
        self.assertTrue(self._websocket.closed)

    def test_request(self) -> None:
        request = JsonRpcRequest(0, 'test', 123)
        self._websocket.reply = json.dumps({'id': 0, 'result': 456})
        with self._connect() as client:
            result = client.request(request.method, request.params)
            self.assertEqual(self._websocket.request, request.to_json())
            self.assertEqual(result, 456)

    def test_task(self) -> None:
        request = JsonRpcRequest(0, 'test', 123)
        self._websocket.reply = json.dumps({'id': 0, 'result': 456})
        with self._connect() as client:
            future = client.task(request.method, request.params)
            self.assertEqual(self._websocket.request, request.to_json())
            self.assertEqual(future.wait_for_result(), 456)

    def test_is_running(self) -> None:
        request = JsonRpcRequest(0, 'test', 123)
        self._websocket.reply = json.dumps({'id': 0, 'result': 456})
        with self._connect() as client:
            future = client.send(request)
            self.assertTrue(client.is_running(0))
            self.assertFalse(client.is_running(1))
            future.wait_for_result()
            self.assertFalse(client.is_running(0))

    def test_send(self) -> None:
        request = JsonRpcRequest(0, 'test', 123)
        self._websocket.reply = json.dumps({'id': 0, 'result': 456})
        with self._connect() as client:
            future = client.send(request)
            self.assertEqual(self._websocket.request, request.to_json())
            self.assertEqual(future.wait_for_result(), 456)

    def test_poll(self) -> None:
        request = JsonRpcRequest(0, 'test', 123)
        self._websocket.reply = json.dumps({'id': 0, 'result': 456})
        with self._connect() as client:
            future = client.send(request)
            self.assertFalse(future.is_ready())
            client.poll()
            self.assertTrue(future.is_ready())
            self.assertEqual(future.wait_for_result(), 456)

    def test_cancel(self) -> None:
        ref = JsonRpcRequest(0, 'cancel', {'id': 123})
        self._websocket.reply = json.dumps({'id': 0, 'result': None})
        with self._connect() as client:
            client.cancel(123)
            self.assertEqual(self._websocket.request, ref.to_json())

    def _connect(self) -> Client:
        return Client(self._websocket, logging.root)


if __name__ == '__main__':
    unittest.main()
