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

import unittest

from brayns.instance.jsonrpc.json_rpc_error import JsonRpcError
from brayns.instance.request_error import RequestError


class TestJsonRpcError(unittest.TestCase):

    def test_from_dict(self) -> None:
        error = JsonRpcError.from_dict({
            'id': 1,
            'error': {
                'message': 'test',
                'code': 2,
                'data': 123
            }
        })
        self.assertEqual(error.id, 1)
        self.assertEqual(error.error.message, 'test')
        self.assertEqual(error.error.code, 2)
        self.assertEqual(error.error.data, 123)

    def test_is_global(self) -> None:
        test = JsonRpcError(None, RequestError(1, 'test'))
        self.assertTrue(test.is_global())


if __name__ == '__main__':
    unittest.main()
