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

from dataclasses import dataclass, field
from typing import Any

from brayns.core.api.json_type import JsonType


@dataclass
class JsonSchema:

    title: str = ''
    description: str = ''
    type: JsonType = JsonType.UNDEFINED
    read_only: bool = False
    write_only: bool = False
    default: Any = None
    minimum: float | None = None
    maximum: float | None = None
    items: JsonSchema | None = None
    min_items: int | None = None
    max_items: int | None = None
    properties: dict[str, JsonSchema] = field(default_factory=dict)
    required: list[str] = field(default_factory=list)
    additional_properties: bool | JsonSchema | None = None
    one_of: list[JsonSchema] = field(default_factory=list)
    enum: list[Any] = field(default_factory=list)

    @staticmethod
    def deserialize(message: dict[str, Any]) -> JsonSchema:
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

    def serialize(self) -> dict[str, Any]:
        message = {}
        if self.title:
            message['title'] = self.title
        if self.description:
            message['description'] = self.description
        if self.type is not JsonType.UNDEFINED:
            message['type'] = self.type.value
        if self.read_only:
            message['readOnly'] = self.read_only
        if self.write_only:
            message['writeOnly'] = self.write_only
        if self.default is not None:
            message['default'] = self.default
        if self.minimum is not None:
            message['minimum'] = self.minimum
        if self.maximum is not None:
            message['maximum'] = self.maximum
        if self.items is not None:
            message['items'] = self.items.serialize()
        if self.min_items is not None:
            message['minItems'] = self.min_items
        if self.max_items is not None:
            message['maxItems'] = self.max_items
        if self.properties:
            message['properties'] = _serialize_properties(self)
        if self.required:
            message['required'] = self.required
        if self.additional_properties is not None:
            message['additionalProperties'] = _serialize_additional(self)
        if self.one_of:
            message['oneOf'] = _serialize_one_of(self)
        if self.enum:
            message['enum'] = self.enum
        return message


def _deserialize_type(message: dict[str, Any]) -> JsonType:
    value = message.get('type')
    if value is None:
        return JsonType.UNDEFINED
    return JsonType(value)


def _deserialize_items(message: dict[str, Any]) -> JsonSchema | None:
    value = message.get('items')
    if value is None:
        return None
    if isinstance(value, dict):
        return JsonSchema.deserialize(value)
    raise TypeError(f'JSON API type error {value}')


def _deserialize_properties(message: dict[str, Any]) -> dict[str, JsonSchema]:
    value = message.get('properties', dict[str, Any]())
    return {
        key: JsonSchema.deserialize(schema)
        for key, schema in value.items()
    }


def _deserialize_additional(message: dict[str, Any]) -> bool | JsonSchema | None:
    value = message.get('additionalProperties')
    if value is None:
        return None
    if value is False:
        return value
    if isinstance(value, dict):
        return JsonSchema.deserialize(value)
    raise TypeError(f'JSON API type error {value}')


def _deserialize_one_of(message: dict[str, Any]) -> list[JsonSchema]:
    value = message.get('oneOf', list[JsonSchema]())
    return [
        JsonSchema.deserialize(one_of)
        for one_of in value
    ]


def _serialize_properties(schema: JsonSchema) -> dict[str, Any]:
    return {
        key: value.serialize()
        for key, value in schema.properties.items()
    }


def _serialize_additional(schema: JsonSchema) -> bool | dict[str, Any] | None:
    properties = schema.additional_properties
    if properties is None:
        return None
    if isinstance(properties, bool):
        return properties
    return properties.serialize()


def _serialize_one_of(schema: JsonSchema) -> list[dict[str, Any]]:
    return [
        one_of.serialize()
        for one_of in schema.one_of
    ]
