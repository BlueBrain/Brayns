# Copyright (c) 2015-2023 EPFL/Blue Brain Project
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

from typing import Any

import brayns


class MockSchema:
    @classmethod
    @property
    def schema(cls) -> brayns.JsonSchema:
        return brayns.JsonSchema(
            title="test1",
            description="test2",
            type=brayns.JsonType.NULL,
            read_only=True,
            write_only=True,
            default=123,
            minimum=0,
            maximum=10,
            items=brayns.JsonSchema(
                type=brayns.JsonType.UNDEFINED,
                additional_properties=False,
            ),
            min_items=1,
            max_items=3,
            properties={
                "1": brayns.JsonSchema(type=brayns.JsonType.BOOLEAN),
                "2": brayns.JsonSchema(type=brayns.JsonType.INTEGER),
            },
            required=["1", "2"],
            additional_properties=False,
            one_of=[
                brayns.JsonSchema(type=brayns.JsonType.NUMBER),
                brayns.JsonSchema(type=brayns.JsonType.STRING),
                brayns.JsonSchema(type=brayns.JsonType.OBJECT),
                brayns.JsonSchema(type=brayns.JsonType.ARRAY),
            ],
            enum=["test", 123],
        )

    @classmethod
    @property
    def message(cls) -> dict[str, Any]:
        return {
            "title": "test1",
            "description": "test2",
            "type": "null",
            "readOnly": True,
            "writeOnly": True,
            "default": 123,
            "minimum": 0,
            "maximum": 10,
            "items": {
                "additionalProperties": False,
            },
            "minItems": 1,
            "maxItems": 3,
            "properties": {
                "1": {"type": "boolean"},
                "2": {"type": "integer"},
            },
            "required": ["1", "2"],
            "additionalProperties": False,
            "oneOf": [
                {"type": "number"},
                {"type": "string"},
                {"type": "object"},
                {"type": "array"},
            ],
            "enum": ["test", 123],
        }
