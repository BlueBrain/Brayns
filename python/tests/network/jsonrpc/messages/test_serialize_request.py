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
from typing import Any

from brayns.network import JsonRpcRequest, serialize_request


class TestSerializeRequest(unittest.TestCase):

    @classmethod
    @property
    def request(cls) -> JsonRpcRequest:
        return JsonRpcRequest(
            id=1,
            method='test',
            params=123,
        )

    @classmethod
    @property
    def message(cls) -> dict[str, Any]:
        return {
            'jsonrpc': '2.0',
            'id': 1,
            'method': 'test',
            'params': 123,
        }

    def test_serialize_request(self) -> None:
        test = serialize_request(self.request)
        self.assertEqual(test, self.message)

    def test_serialize_request_notification(self) -> None:
        notification = self.request
        notification.id = None
        test = serialize_request(notification)
        ref = self.message
        del ref['id']
        self.assertEqual(test, ref)


if __name__ == '__main__':
    unittest.main()
