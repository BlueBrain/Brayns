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

import brayns


class TestJsonRpcRequest(unittest.TestCase):

    def test_to_json_with_id(self) -> None:
        request = brayns.JsonRpcRequest(1, 'test', [1, 2, 3])
        obj = json.loads(request.to_json())
        self.assertEqual(request.id, obj['id'])
        self.assertEqual(request.method, obj['method'])
        self.assertEqual(request.params, obj['params'])

    def test_to_json_notification(self) -> None:
        notification = brayns.JsonRpcRequest(None, 'test', None)
        obj = json.loads(notification.to_json())
        self.assertNotIn('id', obj)
        self.assertEqual(notification.method, obj['method'])
        self.assertNotIn('params', obj)


if __name__ == '__main__':
    unittest.main()
