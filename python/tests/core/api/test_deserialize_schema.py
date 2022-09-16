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
from brayns.core import deserialize_schema


class TestDeserializeSchema(unittest.TestCase):

    @classmethod
    @property
    def message(cls) -> dict[str, Any]:
        return {
            'title': 'test1',
            'description': 'test2',
            'type': 'integer',
            'readOnly': True,
            'writeOnly': True,
            'default': 123,
            'minimum': 0,
            'maximum': 10,
            'items': {
                'type': 'object',
                'additionalProperties': False,
            },
            'minItems': 1,
            'maxItems': 3,
            'properties': {
                '1': {'type': 'string'},
                '2': {'type': 'number'},
            },
            'required': ['1', '2'],
            'additionalProperties': False,
            'oneOf': [
                {'type': 'integer'},
                {'type': 'string'},
            ],
            'enum': ['test', 12],
        }

    def test_deserialize_schema(self) -> None:
        test = deserialize_schema(self.message)
        self.assertEqual(test.title, 'test1')
        self.assertEqual(test.description, 'test2')
        self.assertEqual(test.type, brayns.JsonType.INTEGER)
        self.assertTrue(test.read_only)
        self.assertTrue(test.write_only)
        self.assertEqual(test.default, 123)
        self.assertEqual(test.minimum, 0)
        self.assertEqual(test.maximum, 10)
        self.assertIsNotNone(test.items)
        items = cast(brayns.JsonSchema, test.items)
        self.assertEqual(items.type, brayns.JsonType.OBJECT)
        self.assertFalse(items.additional_properties)
        self.assertEqual(test.min_items, 1)
        self.assertEqual(test.max_items, 3)
        self.assertEqual(test.properties['1'].type, brayns.JsonType.STRING)
        self.assertEqual(test.properties['2'].type, brayns.JsonType.NUMBER)
        self.assertEqual(test.required, ['1', '2'])
        self.assertFalse(test.additional_properties)
        self.assertEqual(test.one_of[0].type, brayns.JsonType.INTEGER)
        self.assertEqual(test.one_of[1].type, brayns.JsonType.STRING)
        self.assertEqual(test.enum, ['test', 12])


if __name__ == '__main__':
    unittest.main()
