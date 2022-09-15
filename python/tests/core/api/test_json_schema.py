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


class TestJsonSchema(unittest.TestCase):

    def setUp(self) -> None:
        self._message = {
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
                'additionalProperties': False
            },
            'minItems': 1,
            'maxItems': 3,
            'properties': {
                '1': {
                    'type': 'string'
                },
                '2': {
                    'type': 'number'
                }
            },
            'required': ['1', '2'],
            'additionalProperties': False,
            'oneOf': [
                {'type': 'integer'},
                {'type': 'string'}
            ],
            'enum': ['test', 12]
        }
        self._schema = brayns.JsonSchema(
            title='test1',
            description='test2',
            type=brayns.JsonType.INTEGER,
            read_only=True,
            write_only=True,
            default=123,
            minimum=0,
            maximum=10,
            items=brayns.JsonSchema(
                type=brayns.JsonType.OBJECT,
                additional_properties=False
            ),
            min_items=1,
            max_items=3,
            properties={
                '1': brayns.JsonSchema(type=brayns.JsonType.STRING),
                '2': brayns.JsonSchema(type=brayns.JsonType.NUMBER)
            },
            required=['1', '2'],
            additional_properties=False,
            one_of=[
                brayns.JsonSchema(type=brayns.JsonType.INTEGER),
                brayns.JsonSchema(type=brayns.JsonType.STRING)
            ],
            enum=['test', 12]
        )

    def test_deserialize(self) -> None:
        test = brayns.JsonSchema.update(self._message)
        self.assertEqual(test, self._schema)

    def test_serialize(self) -> None:
        test = self._schema.to_dict()
        self.assertEqual(test, self._message)


if __name__ == '__main__':
    unittest.main()
