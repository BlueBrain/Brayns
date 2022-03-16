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
from typing import Union

from brayns.client.jsonrpc.json_rpc_client import JsonRpcClient
from brayns.client.jsonrpc.json_rpc_request import JsonRpcRequest
from brayns.client.jsonrpc.json_rpc_task import JsonRpcTask
from brayns.client.request_error import RequestError
from brayns.client.request_progress import RequestProgress
from brayns.client.websocket.web_socket_protocol import WebSocketProtocol


class FakeWebSocket(WebSocketProtocol):

    def __init__(self) -> None:
        self.closed = False
        self.received = []
        self.replies = []

    def close(self) -> None:
        self.closed = True

    def receive(self) -> Union[bytes, str]:
        return self.replies.pop()

    def send(self, data: Union[bytes, str]) -> None:
        self.received.append(data)


class TestJsonRpcClient(unittest.TestCase):

    def setUp(self) -> None:
        self._logger = logging.Logger('Test')
        self._logger.disabled = True
        self._websocket = FakeWebSocket()
        self._sent = []
        self._received = self._websocket.received
        self._client = JsonRpcClient(
            websocket=self._websocket,
            logger=self._logger
        )

    def test_connection(self) -> None:
        with self._client:
            pass
        self.assertTrue(self._websocket.closed)

    def test_notification(self) -> None:
        with self._client:
            task = self._send(JsonRpcRequest(None, 'test', 1))
            self._check_requests()
            self.assertEqual(task.get_result(), None)

    def test_request(self) -> None:
        requests = [
            JsonRpcRequest(1, 'test1', 4),
            JsonRpcRequest(2, 'test', 5),
            JsonRpcRequest(3, 'test', 6)
        ]
        with self._client:
            tasks = [self._send(request) for request in requests]
            self._check_requests()
            for task in tasks:
                self.assertFalse(task.is_ready())
                self.assertFalse(task.has_progress())
            result = 26
            self._reply({
                'id': 1,
                'result': result
            })
            self.assertEqual(tasks[0].get_result(), result)
            progress = RequestProgress('test', 0.5)
            self._reply({
                'params': {
                    'id': 2,
                    'operation': progress.operation,
                    'amount': progress.amount
                }
            })
            self.assertEqual(tasks[1].get_progress(), progress)
            self._reply({
                'id': 2,
                'result': result
            })
            self.assertEqual(tasks[1].get_result(), result)
            error = RequestError('test', 3, 4)
            self._reply({
                'id': 3,
                'error': {
                    'code': error.code,
                    'message': error.message,
                    'data': error.data
                }
            })
            with self.assertRaises(RequestError) as context:
                tasks[2].get_result()
            self.assertEqual(error, context.exception)
            self.assertEqual(len(self._client.get_active_tasks()), 0)

    def _send(self, request: JsonRpcRequest) -> JsonRpcTask:
        self._sent.append(request)
        return self._client.send(request)

    def _reply(self, data: dict) -> None:
        self._websocket.replies.append(json.dumps(data))
        self._client.poll()

    def _check_requests(self) -> None:
        self.assertEqual(len(self._sent), len(self._received))
        for sent, received in zip(self._sent, self._received):
            self._check_request(sent, received)

    def _check_request(self, request: JsonRpcRequest, data: str) -> None:
        self.assertEqual(
            json.loads(request.to_json()),
            json.loads(data)
        )


if __name__ == '__main__':
    unittest.main()
