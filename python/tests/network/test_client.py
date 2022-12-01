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

import logging
import unittest

import brayns
from brayns.network import Client, JsonRpcManager, Listener, serialize_request_to_text

from .jsonrpc.messages.mock_reply import MockReply
from .jsonrpc.messages.mock_request import MockRequest
from .mock_web_socket import MockWebSocket


class TestClient(unittest.TestCase):

    def setUp(self) -> None:
        self._logger = logging.Logger('test')
        self._manager = JsonRpcManager(self._logger)
        self._listener = Listener(self._logger, self._manager)

    def test_context(self) -> None:
        websocket = self._create_websocket()
        with self._connect(websocket):
            self.assertFalse(websocket.closed)
        self.assertTrue(websocket.closed)

    def test_disconnect(self) -> None:
        websocket = self._create_websocket()
        client = self._connect(websocket)
        self.assertFalse(websocket.closed)
        client.disconnect()
        self.assertTrue(websocket.closed)

    def test_request(self) -> None:
        websocket = self._create_websocket()
        request = MockRequest.request
        reply = MockReply.reply
        websocket.text_reply = MockReply.text
        with self._connect(websocket) as client:
            result = client.request(request.method, request.params)
            self.assertEqual(result, reply.result)
        self.assertEqual(websocket.text_request, MockRequest.text)

    def test_request_with_binary(self) -> None:
        websocket = self._create_websocket()
        request = MockRequest.binary_request
        ref = MockReply.binary_reply
        websocket.binary_reply = MockReply.binary
        with self._connect(websocket) as client:
            reply = client.execute(
                request.method,
                request.params,
                request.binary,
            )
            self.assertEqual(reply.result, ref.result)
            self.assertEqual(reply.binary, ref.binary)
        self.assertEqual(websocket.binary_request, MockRequest.binary)

    def test_task(self) -> None:
        websocket = self._create_websocket()
        request = MockRequest.binary_request
        reply = MockReply.binary_reply
        websocket.binary_reply = MockReply.binary
        with self._connect(websocket) as client:
            future = client.task(
                request.method,
                request.params,
                request.binary,
            )
            self.assertEqual(future.wait_for_reply(), reply)
        self.assertEqual(websocket.binary_request, MockRequest.binary)

    def test_is_running(self) -> None:
        websocket = self._create_websocket()
        request = MockRequest.request
        websocket.text_reply = MockReply.text
        with self._connect(websocket) as client:
            future = client.send(request)
            self.assertTrue(client.is_running(0))
            self.assertFalse(client.is_running(1))
            future.wait_for_reply()
            self.assertFalse(client.is_running(0))

    def test_send(self) -> None:
        websocket = self._create_websocket()
        request = MockRequest.request
        websocket.text_reply = MockReply.text
        with self._connect(websocket) as client:
            future = client.send(request)
            self.assertEqual(websocket.text_request, MockRequest.text)
            self.assertEqual(future.wait_for_reply(), MockReply.reply)

    def test_send_binary(self) -> None:
        websocket = self._create_websocket()
        request = MockRequest.binary_request
        websocket.binary_reply = MockReply.binary
        with self._connect(websocket) as client:
            future = client.send(request)
            self.assertEqual(websocket.binary_request, MockRequest.binary)
            self.assertEqual(future.wait_for_reply(), MockReply.binary_reply)

    def test_send_mixed(self) -> None:
        websocket = self._create_websocket()
        request = MockRequest.binary_request
        websocket.text_reply = MockReply.text
        with self._connect(websocket) as client:
            future = client.send(request)
            self.assertEqual(websocket.binary_request, MockRequest.binary)
            self.assertEqual(future.wait_for_reply(), MockReply.reply)

    def test_poll(self) -> None:
        websocket = self._create_websocket()
        request = MockRequest.request
        websocket.text_reply = MockReply.text
        with self._connect(websocket) as client:
            future = client.send(request)
            self.assertFalse(future.is_ready())
            client.poll(block=True)
            self.assertTrue(future.is_ready())
            self.assertEqual(future.wait_for_reply(), MockReply.reply)

    def test_cancel(self) -> None:
        websocket = self._create_websocket()
        request = brayns.JsonRpcRequest(0, 'cancel', {'id': 123})
        ref = serialize_request_to_text(request)
        websocket.text_reply = MockReply.text
        with self._connect(websocket) as client:
            client.cancel(123)
            self.assertEqual(websocket.text_request, ref)

    def _create_websocket(self) -> MockWebSocket:
        return MockWebSocket(self._listener)

    def _connect(self, websocket: MockWebSocket) -> Client:
        return Client(websocket, self._logger, self._manager)
