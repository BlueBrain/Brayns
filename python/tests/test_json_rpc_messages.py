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

from brayns.client.jsonrpc.json_rpc_error import JsonRpcError
from brayns.client.jsonrpc.json_rpc_progress import JsonRpcProgress
from brayns.client.jsonrpc.json_rpc_reply import JsonRpcReply
from brayns.client.jsonrpc.json_rpc_request import JsonRpcRequest
from brayns.client.request_error import RequestError


class TestJsonRpcMessages(unittest.TestCase):

    def test_error(self) -> None:
        error = JsonRpcError.from_dict({
            'id': 1,
            'error': {
                'message': 'test',
                'code': 2,
                'data': [1, 2, 3]
            }
        })
        self.assertEqual(error.id, 1)
        self.assertEqual(error.error, RequestError(
            message='test',
            code=2,
            data=[1, 2, 3]
        ))

    def test_progress(self) -> None:
        progress = JsonRpcProgress.from_dict({
            'params': {
                'id': 1,
                'operation': 'test',
                'amount': 0.5
            }
        })
        self.assertEqual(progress.id, 1)
        self.assertEqual(progress.params.operation, 'test')
        self.assertEqual(progress.params.amount, 0.5)

    def test_reply(self) -> None:
        reply = JsonRpcReply.from_dict({
            'id': 1,
            'result': 12
        })
        self.assertEqual(reply.id, 1)
        self.assertEqual(reply.result, 12)

    def test_request(self) -> None:
        request = JsonRpcRequest(1, 'test', [1, 2, 3])
        message = json.loads(request.to_json())
        self.assertEqual(request.id, message['id'])
        self.assertEqual(request.method, message['method'])
        self.assertEqual(request.params, message['params'])
        notification = JsonRpcRequest(None, 'test', None)
        message = json.loads(notification.to_json())
        self.assertEqual(len(message), 2)
        self.assertEqual(message['method'], notification.method)


if __name__ == '__main__':
    unittest.main()
