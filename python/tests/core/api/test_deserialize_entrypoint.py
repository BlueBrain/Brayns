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
from typing import Any, cast

import brayns
from brayns.core import deserialize_entrypoint


class TestDeserializeEntrypoint(unittest.TestCase):

    @classmethod
    @property
    def test_message(cls) -> dict[str, Any]:
        return {
            'title': 'test',
            'description': 'test2',
            'plugin': 'test3',
            'async': True,
            'params': {
                'type': 'object',
            },
            'returns': {
                'type': 'array',
            },
        }

    def test_deserialize_entrypoint(self) -> None:
        test = deserialize_entrypoint(self.test_message)
        self.assertEqual(test.method, 'test')
        self.assertEqual(test.description, 'test2')
        self.assertEqual(test.plugin, 'test3')
        self.assertTrue(test.asynchronous)
        self.assertIsNotNone(test.params)
        params = cast(brayns.JsonSchema, test.params)
        self.assertIs(params.type, brayns.JsonType.OBJECT)
        self.assertIsNotNone(test.result)
        result = cast(brayns.JsonSchema, test.result)
        self.assertIs(result.type, brayns.JsonType.ARRAY)


if __name__ == '__main__':
    unittest.main()
