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

from brayns.client.client import Client
from brayns.client.request_error import RequestError

from helpers.web_socket_connection import WebSocketConnection
from helpers.web_socket_server import WebSocketServer


class TestClient(unittest.TestCase):

    def setUp(self) -> None:
        self._uri = 'localhost:5000'
        self._server = WebSocketServer(self._handle_connection, self._uri)
        self._request = None
        self._method = 'test'
        self._params = 123
        self._result = 456

    def tearDown(self) -> None:
        self._client.disconnect()
        self._server.stop()

    def test_request(self) -> None:
        self._handle_request = self._single_reply
        self._client = Client(self._uri)
        result = self._client.request(self._method, self._params)
        self._check_request()
        self.assertEqual(result, self._result)

    def test_error(self) -> None:
        self._error = RequestError('test', 22, [1, 2, 3])
        self._handle_request = self._single_error
        self._client = Client(self._uri)
        with self.assertRaises(RequestError) as context:
            self._client.request(self._method, self._params)
        self.assertEqual(context.exception, self._error)

    def test_task(self) -> None:
        self._progress_count = 4
        self._operation = 'bla'
        self._handle_request = self._task_reply
        self._client = Client(self._uri)
        task = self._client.task(self._method, self._params)
        count = 0
        for progress in task:
            self.assertEqual(progress.operation, self._operation)
            self.assertGreaterEqual(progress.amount, 0.0)
            self.assertLessEqual(progress.amount, 1.0)
            count += 1
        result = task.wait_for_result()
        self._check_request()
        self.assertEqual(result, self._result)

    def test_cancel(self) -> None:
        self._cancel = None
        self._handle_request = self._cancel_reply
        self._client = Client(self._uri)
        task = self._client.task(self._method, self._params)
        task.cancel()
        with self.assertRaises(RequestError):
            task.wait_for_result()
        self.assertEqual(self._cancel['params']['id'], self._request['id'])
        self.assertEqual(self._cancel['params']['id'], self._request['id'])

    def _check_request(self) -> None:
        self.assertEqual(self._request['method'], self._method)
        self.assertEqual(self._request['params'], self._params)

    async def _send_reply(self, connection: WebSocketConnection) -> None:
        await connection.send(json.dumps({
            'id': self._request['id'],
            'result': self._result
        }))

    async def _handle_connection(self, connection: WebSocketConnection) -> None:
        await self._handle_request(connection)

    async def _single_reply(self, connection: WebSocketConnection) -> None:
        self._request = json.loads(await connection.receive())
        await self._send_reply(connection)

    async def _single_error(self, connection: WebSocketConnection) -> None:
        self._request = json.loads(await connection.receive())
        await connection.send(json.dumps({
            'id': self._request['id'],
            'error': {
                'code': self._error.code,
                'message': self._error.message,
                'data': self._error.data
            }
        }))

    async def _task_reply(self, connection: WebSocketConnection) -> None:
        self._request = json.loads(await connection.receive())
        progress = {
            'params': {
                'id': self._request['id'],
                'operation': self._operation,
                'amount': 0.0
            }
        }
        for _ in range(self._progress_count):
            await connection.send(json.dumps(progress))
            progress['params']['amount'] += 1 / self._progress_count
        await self._send_reply(connection)

    async def _cancel_reply(self, connection: WebSocketConnection) -> None:
        self._request = json.loads(await connection.receive())
        self._cancel = json.loads(await connection.receive())
        await connection.send(json.dumps({
            'id': self._cancel['id'],
            'result': None
        }))
        await connection.send(json.dumps({
            'id': self._request['id'],
            'error': {
                'code': 0,
                'message': 'Test cancel'
            }
        }))


if __name__ == '__main__':
    unittest.main()
