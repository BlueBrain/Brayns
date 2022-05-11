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

from brayns.instance.jsonrpc.json_rpc_client import JsonRpcClient
from brayns.instance.jsonrpc.json_rpc_request import JsonRpcRequest
from tests.instance.websocket.mock_web_socket import MockWebSocket


class TestJsonRpcClient(unittest.TestCase):

    def setUp(self) -> None:
        self._logger = logging.Logger('Test')
        self._websocket = MockWebSocket()

    def test_context(self) -> None:
        with self._connect():
            pass
        self.assertTrue(self._websocket.closed)

    def test_disconnect(self) -> None:
        client = self._connect()
        client.disconnect()
        self.assertTrue(self._websocket.closed)

    def test_send(self) -> None:
        request = JsonRpcRequest(0, 'test', 12)
        with self._connect() as client:
            client.send(request)
            self.assertEqual(self._websocket.request, request.to_json())

    def test_poll(self) -> None:
        request = JsonRpcRequest(0, 'test', 123)
        self._websocket.reply = json.dumps({'id': 0, 'result': 456})
        with self._connect() as client:
            task = client.send(request)
            self.assertFalse(task.is_ready())
            client.poll()
            test = task.get_result()
            self.assertEqual(test, 456)

    def test_poll_notification(self) -> None:
        request = JsonRpcRequest(None, 'test', 123)
        with self._connect() as client:
            task = client.send(request)
            self.assertEqual(task.get_result(), None)

    def _connect(self) -> JsonRpcClient:
        return JsonRpcClient(
            websocket=self._websocket,
            logger=self._logger
        )


if __name__ == '__main__':
    unittest.main()
