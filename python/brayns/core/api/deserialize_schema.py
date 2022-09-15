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

from __future__ import annotations

from typing import Any

from .json_schema import JsonSchema
from .json_type import JsonType


def deserialize_schema(message: dict[str, Any]) -> JsonSchema:
    return JsonSchema(
        title=message.get('title', ''),
        description=message.get('description', ''),
        type=_deserialize_type(message),
        read_only=message.get('readOnly', False),
        write_only=message.get('writeOnly', False),
        default=message.get('default'),
        minimum=message.get('minimum'),
        maximum=message.get('maximum'),
        items=_deserialize_items(message),
        min_items=message.get('minItems'),
        max_items=message.get('maxItems'),
        properties=_deserialize_properties(message),
        required=message.get('required', []),
        additional_properties=_deserialize_additional(message),
        one_of=_deserialize_one_of(message),
        enum=message.get('enum', []),
    )


def _deserialize_type(obj: dict[str, Any]) -> JsonType:
    value = obj.get('type')
    if value is None:
        return JsonType.UNDEFINED
    return JsonType(value)


def _deserialize_items(obj: dict[str, Any]) -> JsonSchema | None:
    value = obj.get('items')
    if value is None:
        return None
    if isinstance(value, dict):
        return deserialize_schema(value)
    raise TypeError(f'JSON API type error {value}')


def _deserialize_properties(obj: dict[str, Any]) -> dict[str, JsonSchema]:
    value = obj.get('properties', dict[str, Any]())
    return {
        key: deserialize_schema(schema)
        for key, schema in value.items()
    }


def _deserialize_additional(obj: dict[str, Any]) -> bool | JsonSchema | None:
    value = obj.get('additionalProperties')
    if value is None:
        return None
    if value is False:
        return value
    if isinstance(value, dict):
        return deserialize_schema(value)
    raise TypeError(f'JSON API type error {value}')


def _deserialize_one_of(obj: dict[str, Any]) -> list[JsonSchema]:
    value = obj.get('oneOf', list[JsonSchema]())
    return [
        deserialize_schema(one_of)
        for one_of in value
    ]
