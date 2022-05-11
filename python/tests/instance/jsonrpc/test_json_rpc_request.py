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

from brayns.instance.jsonrpc.json_rpc_request import JsonRpcRequest


class TestJsonRpcRequest(unittest.TestCase):

    def test_to_json(self) -> None:
        request = JsonRpcRequest(1, 'test', [1, 2, 3])
        message = json.loads(request.to_json())
        self.assertEqual(request.id, message['id'])
        self.assertEqual(request.method, message['method'])
        self.assertEqual(request.params, message['params'])
        notification = JsonRpcRequest(None, 'test', None)
        message = json.loads(notification.to_json())
        self.assertNotIn('id', message)
        self.assertEqual(notification.method, message['method'])
        self.assertNotIn('params', message)

    def test_is_notification(self) -> None:
        test = JsonRpcRequest(None, 'test')
        self.assertTrue(test.is_notification())


if __name__ == '__main__':
    unittest.main()
