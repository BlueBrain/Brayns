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

from brayns.core.api.entrypoint import Entrypoint
from brayns.core.api.json_schema import JsonSchema
from brayns.core.api.json_type import JsonType
from tests.instance.mock_instance import MockInstance


class TestEntrypoint(unittest.TestCase):

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
        self._entrypoint = Entrypoint(
            method='test',
            description='test2',
            plugin='stuff',
            asynchronous=True,
            params=JsonSchema(type=JsonType.OBJECT),
            result=JsonSchema(type=JsonType.ARRAY)
        )

    def test_get_all_methods(self) -> None:
        methods = ['test1', 'test2']
        instance = MockInstance(methods)
        tests = Entrypoint.get_all_methods(instance)
        self.assertEqual(instance.method, 'registry')
        self.assertEqual(instance.params, None)
        self.assertEqual(tests, methods)

    def test_from_method(self) -> None:
        instance = MockInstance(self._message)
        test = Entrypoint.from_method(instance, 'test')
        self.assertEqual(instance.method, 'schema')
        self.assertEqual(instance.params, {'endpoint': 'test'})
        self.assertEqual(test, self._entrypoint)

    def test_deserialize(self) -> None:
        test = Entrypoint.deserialize(self._message)
        ref = self._entrypoint
        self.assertEqual(test, ref)


if __name__ == '__main__':
    unittest.main()
