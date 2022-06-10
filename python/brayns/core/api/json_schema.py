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

from dataclasses import dataclass, field
from typing import Any, Optional, Union

from brayns.core.api.json_type import JsonType


@dataclass
class JsonSchema:

    title: str = ''
    description: str = ''
    type: JsonType = JsonType.UNDEFINED
    read_only: bool = False
    write_only: bool = False
    default: Any = None
    minimum: Optional[float] = None
    maximum: Optional[float] = None
    items: Optional['JsonSchema'] = None
    min_items: Optional[int] = None
    max_items: Optional[int] = None
    properties: dict[str, 'JsonSchema'] = field(default_factory=dict)
    required: list[str] = field(default_factory=list)
    additional_properties: Optional[Union[bool, 'JsonSchema']] = None
    one_of: list['JsonSchema'] = field(default_factory=list)
    enum: list[Any] = field(default_factory=list)

    @staticmethod
    def deserialize(message: dict) -> 'JsonSchema':
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
            enum=message.get('enum', [])
        )

    def serialize(self) -> dict:
        result = {}
        if self.title:
            result['title'] = self.title
        if self.description:
            result['description'] = self.description
        if self.type is not JsonType.UNDEFINED:
            result['type'] = self.type.value
        if self.read_only:
            result['readOnly'] = self.read_only
        if self.write_only:
            result['writeOnly'] = self.write_only
        if self.default is not None:
            result['default'] = self.default
        if self.minimum is not None:
            result['minimum'] = self.minimum
        if self.maximum is not None:
            result['maximum'] = self.maximum
        if self.items is not None:
            result['items'] = self.items.serialize()
        if self.min_items is not None:
            result['minItems'] = self.min_items
        if self.max_items is not None:
            result['maxItems'] = self.max_items
        if self.properties:
            result['properties'] = _serialize_properties(self)
        if self.required:
            result['required'] = self.required
        if self.additional_properties is not None:
            result['additionalProperties'] = _serialize_additional(self)
        if self.one_of:
            result['oneOf'] = _serialize_one_of(self)
        if self.enum:
            result['enum'] = self.enum
        return result


def _deserialize_type(message: dict) -> JsonType:
    return JsonType(message.get('type', JsonType.UNDEFINED))


def _deserialize_items(message: dict) -> JsonSchema:
    value = message.get('items')
    if value is None:
        return None
    return JsonSchema.deserialize(value)


def _deserialize_properties(message: dict) -> dict[str, JsonSchema]:
    return {
        key: JsonSchema.deserialize(value)
        for key, value in message.get('properties', {}).items()
    }


def _serialize_properties(schema: JsonSchema) -> dict[str, dict]:
    return {
        key: value.serialize()
        for key, value in schema.properties.items()
    }


def _deserialize_additional(message: dict) -> Optional[Union[bool, JsonSchema]]:
    value = message.get('additionalProperties')
    if value is None:
        return None
    if value is False:
        return False
    return JsonSchema.deserialize(value)


def _serialize_additional(schema: JsonSchema) -> Union[bool, dict]:
    if schema.additional_properties is False:
        return False
    return schema.additional_properties.serialize()


def _deserialize_one_of(message: dict) -> list[JsonSchema]:
    return [
        JsonSchema.deserialize(one_of)
        for one_of in message.get('oneOf', [])
    ]


def _serialize_one_of(schema: JsonSchema) -> list[dict]:
    return [
        one_of.serialize()
        for one_of in schema.one_of
    ]
