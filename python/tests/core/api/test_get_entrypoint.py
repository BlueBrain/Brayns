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

import brayns
from tests.instance.mock_instance import MockInstance


class TestGetEntrypoint(unittest.TestCase):

    def setUp(self) -> None:
        self._message = {
            'title': 'test',
            'description': 'test2',
            'plugin': 'stuff',
            'async': True,
            'params': {
                'type': 'object'
            },
            'returns': {
                'type': 'array'
            }
        }
        self._entrypoint = brayns.Entrypoint(
            method='test',
            description='test2',
            plugin='stuff',
            asynchronous=True,
            params=brayns.JsonSchema(type=brayns.JsonType.OBJECT),
            result=brayns.JsonSchema(type=brayns.JsonType.ARRAY)
        )

    def test_get_entrypoint(self) -> None:
        instance = MockInstance(self._message)
        test = brayns.get_entrypoint(instance, 'test')
        self.assertEqual(instance.method, 'schema')
        self.assertEqual(instance.params, {'endpoint': 'test'})
        self.assertEqual(test, self._entrypoint)


if __name__ == '__main__':
    unittest.main()
