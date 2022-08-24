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

from brayns.network.jsonrpc.json_rpc_dispatcher import JsonRpcDispatcher
from brayns.network.jsonrpc.json_rpc_error import JsonRpcError
from brayns.network.jsonrpc.json_rpc_progress import JsonRpcProgress
from brayns.network.jsonrpc.json_rpc_reply import JsonRpcReply
from .mock_json_rpc_listener import MockJsonRpcListener


class TestJsonRpcDispatcher(unittest.TestCase):

    def setUp(self) -> None:
        self._listener = MockJsonRpcListener()
        self._dispatcher = JsonRpcDispatcher(self._listener)

    def test_dispatch_reply(self) -> None:
        reply = {
            'id': 1,
            'result': [1, 2, 3]
        }
        self._dispatcher.dispatch(json.dumps(reply))
        test = self._listener.get_data()
        ref = JsonRpcReply.deserialize(reply)
        self.assertEqual(test, ref)

    def test_dispatch_error(self) -> None:
        error = {
            'id': 1,
            'error': {
                'code': 1,
                'message': 'test'
            }
        }
        self._dispatcher.dispatch(json.dumps(error))
        test = self._listener.get_data()
        ref = JsonRpcError.deserialize(error)
        self.assertEqual(test, ref)

    def test_dispatch_progress(self) -> None:
        progress = {
            'params': {
                'id': 1,
                'operation': 'test',
                'amount': 0.5
            }
        }
        self._dispatcher.dispatch(json.dumps(progress))
        test = self._listener.get_data()
        ref = JsonRpcProgress.deserialize(progress)
        self.assertEqual(test, ref)

    def test_dispatch_invalid_frame(self) -> None:
        invalid_frame = 'sdfbxcbxbcv'
        self._dispatcher.dispatch(invalid_frame)
        data, e = self._listener.get_data()
        self.assertEqual(data, invalid_frame)
        self.assertIsInstance(e, Exception)


if __name__ == '__main__':
    unittest.main()
