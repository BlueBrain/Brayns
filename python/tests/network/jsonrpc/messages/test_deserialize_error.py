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

import brayns
from brayns.network import JsonRpcError, deserialize_error


class TestDeserializeError(unittest.TestCase):

    @classmethod
    @property
    def error(cls) -> JsonRpcError:
        return JsonRpcError(
            id=1,
            error=brayns.RequestError(
                code=2,
                message='test',
                data=123,
            ),
        )

    @classmethod
    @property
    def message(cls) -> dict[str, Any]:
        return {
            'id': 1,
            'error': {
                'code': 2,
                'message': 'test',
                'data': 123,
            },
        }

    def test_deserialize_error(self) -> None:
        test = deserialize_error(self.message)
        self.assertEqual(test, self.error)


if __name__ == '__main__':
    unittest.main()
