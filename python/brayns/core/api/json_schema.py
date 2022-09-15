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

from brayns.network import JsonRpcMessage

from .json_type import JsonType


@dataclass
class JsonSchema(JsonRpcMessage):
    """JSON schema object to describe JSON data.

    :param title: Title (class name).
    :type title: str
    :param description: Human readable description.
    :type description: str
    :param type: JSON type (null, boolean, number, string, object, ...).
    :type type: JsonType
    :param read_only: If true, this field cannot be assigned.
    :type read_only: bool
    :param write_only: If true, this field cannot get retreived.
    :type write_only: bool
    :param default: Optional default value.
    :type default: Any
    :param minimum: Optional minimum value for numbers.
    :type minimum: float | None
    :param maximum: Optional maximum value for numbers.
    :type maximum: float | None
    :param items: Optional item schema value for arrays.
    :type items: JsonSchema | None
    :param min_items: Optional minimum item count for arrays.
    :type min_items: int | None
    :param max_items: Optional maximum item count for arrays.
    :type max_items: int | None
    :param properties: Optional fixed properties schema for objects.
    :type properties: dict[str, JsonSchema]
    :param required: Optional list of required properties for objects.
    :type required: list[str]
    :param additional_properties: Schema of additional properties for objects.
        If False then no additional properties are allowed (only fixed).
        If None then any additional properties are allowed (usually ignored).
    :type additional_properties: bool | JsonSchema | None
    :param one_of: If not empty, the schema is a union of several schemas.
    :type one_of: list[JsonSchema]
    :param enum: If not empty, the message must be in these values.
    :type enum: list[Any]
    """

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

    def to_dict(self) -> dict[str, Any]:
        """Low level API to serialize to JSON."""
        obj = dict[str, Any]()
        if self.title:
            obj['title'] = self.title
        if self.description:
            obj['description'] = self.description
        if self.type is not JsonType.UNDEFINED:
            obj['type'] = self.type.value
        if self.read_only:
            obj['readOnly'] = self.read_only
        if self.write_only:
            obj['writeOnly'] = self.write_only
        if self.default is not None:
            obj['default'] = self.default
        if self.minimum is not None:
            obj['minimum'] = self.minimum
        if self.maximum is not None:
            obj['maximum'] = self.maximum
        if self.items is not None:
            obj['items'] = self.items.to_dict()
        if self.min_items is not None:
            obj['minItems'] = self.min_items
        if self.max_items is not None:
            obj['maxItems'] = self.max_items
        if self.properties:
            obj['properties'] = _serialize_properties(self)
        if self.required:
            obj['required'] = self.required
        if self.additional_properties is not None:
            obj['additionalProperties'] = _serialize_additional(self)
        if self.one_of:
            obj['oneOf'] = _serialize_one_of(self)
        if self.enum:
            obj['enum'] = self.enum
        return obj

    def update(self, obj: dict[str, Any]) -> None:
        """Low level API to deserialize from JSON."""
        self.title = obj.get('title', '')
        self.description = obj.get('description', '')
        self.type = _deserialize_type(obj)
        self.read_only = obj.get('readOnly', False)
        self.write_only = obj.get('writeOnly', False)
        self.default = obj.get('default')
        self.minimum = obj.get('minimum')
        self.maximum = obj.get('maximum')
        self.items = _deserialize_items(obj)
        self.min_items = obj.get('minItems')
        self.max_items = obj.get('maxItems')
        self.properties = _deserialize_properties(obj)
        self.required = obj.get('required', [])
        self.additional_properties = _deserialize_additional(obj)
        self.one_of = _deserialize_one_of(obj)
        self.enum = obj.get('enum', [])


def _serialize_properties(schema: JsonSchema) -> dict[str, Any]:
    return {
        key: value.to_dict()
        for key, value in schema.properties.items()
    }


def _serialize_additional(schema: JsonSchema) -> bool | dict[str, Any] | None:
    properties = schema.additional_properties
    if properties is None:
        return None
    if isinstance(properties, bool):
        return properties
    return properties.to_dict()


def _serialize_one_of(schema: JsonSchema) -> list[dict[str, Any]]:
    return [
        one_of.to_dict()
        for one_of in schema.one_of
    ]


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
        return JsonSchema.from_dict(value)
    raise TypeError(f'JSON API type error {value}')


def _deserialize_properties(obj: dict[str, Any]) -> dict[str, JsonSchema]:
    value = obj.get('properties', dict[str, Any]())
    return {
        key: JsonSchema.from_dict(schema)
        for key, schema in value.items()
    }


def _deserialize_additional(obj: dict[str, Any]) -> bool | JsonSchema | None:
    value = obj.get('additionalProperties')
    if value is None:
        return None
    if value is False:
        return value
    if isinstance(value, dict):
        return JsonSchema.from_dict(value)
    raise TypeError(f'JSON API type error {value}')


def _deserialize_one_of(obj: dict[str, Any]) -> list[JsonSchema]:
    value = obj.get('oneOf', list[JsonSchema]())
    return [
        JsonSchema.from_dict(one_of)
        for one_of in value
    ]
