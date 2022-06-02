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

from brayns.core.api.json_schema import JsonSchema
from brayns.core.api.json_type import JsonType


class TestJsonSchema(unittest.TestCase):

    def test_deserialize(self) -> None:
        message = {
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
            'enums': ['test', 12]
        }
        ref = JsonSchema(
            title='test1',
            description='test2',
            type=JsonType.INTEGER,
            read_only=True,
            write_only=True,
            default_value=123,
            minimum=0,
            maximum=10,
            items=JsonSchema(type=JsonType.OBJECT),
            min_items=1,
            max_items=3,
            properties={
                '1': JsonSchema(type=JsonType.STRING),
                '2': JsonSchema(type=JsonType.NUMBER)
            },
            required=['1', '2'],
            additional_properties=None,
            one_of=[
                JsonSchema(type=JsonType.INTEGER),
                JsonSchema(type=JsonType.STRING)
            ],
            enums=['test', 12]
        )
        test = JsonSchema.deserialize(message)
        self.assertEqual(test, ref)


if __name__ == '__main__':
    unittest.main()
