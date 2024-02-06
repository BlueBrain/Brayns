# Copyright (c) 2015-2024 EPFL/Blue Brain Project
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
from brayns.network.client import Client
from brayns.network.jsonrpc import JsonRpcManager, serialize_request_to_text
from brayns.network.listener import Listener
from brayns.network.websocket import WebSocket, WebSocketListener

from .jsonrpc.mock_messages import (
    mock_reply,
    mock_reply_binary,
    mock_reply_text,
    mock_request,
    mock_request_binary,
    mock_request_text,
)


class MockWebSocket(WebSocket):
    def __init__(self, listener: WebSocketListener) -> None:
        self.binary_request = b""
        self.binary_reply = b""
        self.text_request = ""
        self.text_reply = ""
        self._listener = listener
        self._closed = False

    @property
    def closed(self) -> bool:
        return self._closed

    def close(self) -> None:
        self._closed = True

    def poll(self, *_) -> None:
        if self.binary_reply:
            self._listener.on_binary(self.binary_reply)
        if self.text_reply:
            self._listener.on_text(self.text_reply)

    def send_binary(self, data: bytes) -> None:
        self.binary_request = data

    def send_text(self, data: str) -> None:
        self.text_request = data


class TestClient(unittest.TestCase):
    def setUp(self) -> None:
        self._logger = logging.Logger("test")
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
        request = mock_request()
        reply = mock_reply()
        websocket.text_reply = mock_reply_text()
        with self._connect(websocket) as client:
            result = client.request(request.method, request.params)
            self.assertEqual(result, reply.result)
        self.assertEqual(websocket.text_request, mock_request_text())

    def test_request_with_binary(self) -> None:
        websocket = self._create_websocket()
        request = mock_request(binary=True)
        ref = mock_reply(binary=True)
        websocket.binary_reply = mock_reply_binary()
        with self._connect(websocket) as client:
            reply = client.execute(
                request.method,
                request.params,
                request.binary,
            )
            self.assertEqual(reply.result, ref.result)
            self.assertEqual(reply.binary, ref.binary)
        self.assertEqual(websocket.binary_request, mock_request_binary())

    def test_task(self) -> None:
        websocket = self._create_websocket()
        request = mock_request(binary=True)
        reply = mock_reply(binary=True)
        websocket.binary_reply = mock_reply_binary()
        with self._connect(websocket) as client:
            future = client.task(
                request.method,
                request.params,
                request.binary,
            )
            self.assertEqual(future.wait_for_reply(), reply)
        self.assertEqual(websocket.binary_request, mock_request_binary())

    def test_is_running(self) -> None:
        websocket = self._create_websocket()
        request = mock_request()
        websocket.text_reply = mock_reply_text()
        with self._connect(websocket) as client:
            future = client.send(request)
            self.assertTrue(client.is_running(0))
            self.assertFalse(client.is_running(1))
            future.wait_for_reply()
            self.assertFalse(client.is_running(0))

    def test_send(self) -> None:
        websocket = self._create_websocket()
        request = mock_request()
        websocket.text_reply = mock_reply_text()
        with self._connect(websocket) as client:
            future = client.send(request)
            self.assertEqual(websocket.text_request, mock_request_text())
            self.assertEqual(future.wait_for_reply(), mock_reply())

    def test_send_binary(self) -> None:
        websocket = self._create_websocket()
        request = mock_request(binary=True)
        websocket.binary_reply = mock_reply_binary()
        with self._connect(websocket) as client:
            future = client.send(request)
            self.assertEqual(websocket.binary_request, mock_request_binary())
            self.assertEqual(future.wait_for_reply(), mock_reply(binary=True))

    def test_send_mixed(self) -> None:
        websocket = self._create_websocket()
        request = mock_request(binary=True)
        websocket.text_reply = mock_reply_text()
        with self._connect(websocket) as client:
            future = client.send(request)
            self.assertEqual(websocket.binary_request, mock_request_binary())
            self.assertEqual(future.wait_for_reply(), mock_reply())

    def test_poll(self) -> None:
        websocket = self._create_websocket()
        request = mock_request()
        websocket.text_reply = mock_reply_text()
        with self._connect(websocket) as client:
            future = client.send(request)
            self.assertFalse(future.is_ready())
            client.poll(block=True)
            self.assertTrue(future.is_ready())
            self.assertEqual(future.wait_for_reply(), mock_reply())

    def test_cancel(self) -> None:
        websocket = self._create_websocket()
        request = brayns.JsonRpcRequest(0, "cancel", {"id": 123})
        ref = serialize_request_to_text(request)
        websocket.text_reply = mock_reply_text()
        with self._connect(websocket) as client:
            client.cancel(123)
            self.assertEqual(websocket.text_request, ref)

    def _create_websocket(self) -> MockWebSocket:
        return MockWebSocket(self._listener)

    def _connect(self, websocket: MockWebSocket) -> Client:
        return Client(websocket, self._logger, self._manager)
