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
from brayns.instance.jsonrpc.json_rpc_client import JsonRpcClient
from brayns.instance.jsonrpc.json_rpc_request import JsonRpcRequest
from brayns.instance.request_error import RequestError
from brayns.instance.request_progress import RequestProgress
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

    def test_request_error(self) -> None:
        self._websocket.reply = json.dumps({
            'id': 0,
            'error': {
                'code': 0,
                'message': 'test'
            }
        })
        with self._connect() as client:
            with self.assertRaises(RequestError) as context:
                client.request('test', 123)
            self.assertEqual(context.exception, RequestError(0, 'test'))

    def test_task(self) -> None:
        request = JsonRpcRequest(0, 'test', 123)
        self._websocket.reply = json.dumps({'id': 0, 'result': 456})
        with self._connect() as client:
            future = client.task(request.method, request.params)
            self.assertEqual(self._websocket.request, request.to_json())
            self.assertEqual(future.wait_for_result(), 456)

    def test_task_cancel(self) -> None:
        with self._connect() as client:
            future = client.task('test')
            self._websocket.reply = json.dumps({'id': 1, 'result': None})
            future.cancel()
            self.assertEqual(json.loads(self._websocket.request), {
                'jsonrpc': '2.0',
                'id': 1,
                'method': 'cancel',
                'params': {
                    'id': 0
                }
            })

    def test_task_progress(self) -> None:
        with self._connect() as client:
            future = client.task('test')
            progress = RequestProgress('test', 0.5)
            self._websocket.reply = json.dumps({
                'params': {
                    'id': 0,
                    'operation': 'test',
                    'amount': 0.5
                }
            })
            self.assertEqual(next(iter(future)), progress)

    def _connect(self) -> Client:
        return Client(
            JsonRpcClient(
                websocket=self._websocket,
                logger=logging.Logger('Test')
            )
        )


if __name__ == '__main__':
    unittest.main()
