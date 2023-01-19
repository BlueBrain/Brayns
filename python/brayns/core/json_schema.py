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

from __future__ import annotations

from dataclasses import dataclass, field
from enum import Enum
from typing import Any


class JsonType(Enum):
    """Enumeration of available JSON types.

    The value is the string from the JSON schema standard.

    :param UNDEFINED: Any type is allowed.
    :param NULL: No types are allowed.
    :param BOOLEAN: Boolean.
    :param INTEGER: Integral number.
    :param NUMBER: Any number.
    :param STRING: String.
    :param ARRAY: Array.
    :param OBJECT: Object.
    """

    UNDEFINED = "undefined"
    NULL = "null"
    BOOLEAN = "boolean"
    INTEGER = "integer"
    NUMBER = "number"
    STRING = "string"
    ARRAY = "array"
    OBJECT = "object"


@dataclass
class JsonSchema:
    """JSON schema describing JSON-RPC message format.

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

    title: str = ""
    description: str = ""
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


def deserialize_schema(message: dict[str, Any]) -> JsonSchema:
    """Deserialize dictionary to JSON schema.

    :param message: Parsed JSON object.
    :type message: dict[str, Any]
    :return: JSON schema with supported fields.
    :rtype: JsonSchema
    """
    return JsonSchema(
        title=message.get("title", ""),
        description=message.get("description", ""),
        type=_deserialize_type(message),
        read_only=message.get("readOnly", False),
        write_only=message.get("writeOnly", False),
        default=message.get("default"),
        minimum=message.get("minimum"),
        maximum=message.get("maximum"),
        items=_deserialize_items(message),
        min_items=message.get("minItems"),
        max_items=message.get("maxItems"),
        properties=_deserialize_properties(message),
        required=message.get("required", []),
        additional_properties=_deserialize_additional(message),
        one_of=_deserialize_one_of(message),
        enum=message.get("enum", []),
    )


def serialize_schema(schema: JsonSchema) -> dict[str, Any]:
    """Serialize JSON schema to dictionary.

    :param schema: JSON schema.
    :type schema: JsonSchema
    :return: Dictionary that can be stringified to raw JSON.
    :rtype: dict[str, Any]
    """
    message = dict[str, Any]()
    if schema.title:
        message["title"] = schema.title
    if schema.description:
        message["description"] = schema.description
    if schema.type is not JsonType.UNDEFINED:
        message["type"] = schema.type.value
    if schema.read_only:
        message["readOnly"] = schema.read_only
    if schema.write_only:
        message["writeOnly"] = schema.write_only
    if schema.default is not None:
        message["default"] = schema.default
    if schema.minimum is not None:
        message["minimum"] = schema.minimum
    if schema.maximum is not None:
        message["maximum"] = schema.maximum
    if schema.items is not None:
        message["items"] = serialize_schema(schema.items)
    if schema.min_items is not None:
        message["minItems"] = schema.min_items
    if schema.max_items is not None:
        message["maxItems"] = schema.max_items
    if schema.properties:
        message["properties"] = _serialize_properties(schema)
    if schema.required:
        message["required"] = schema.required
    if schema.additional_properties is not None:
        message["additionalProperties"] = _serialize_additional(schema)
    if schema.one_of:
        message["oneOf"] = _serialize_one_of(schema)
    if schema.enum:
        message["enum"] = schema.enum
    return message


def _deserialize_type(obj: dict[str, Any]) -> JsonType:
    value = obj.get("type")
    if value is None:
        return JsonType.UNDEFINED
    return JsonType(value)


def _deserialize_items(obj: dict[str, Any]) -> JsonSchema | None:
    value = obj.get("items")
    if value is None:
        return None
    if isinstance(value, dict):
        return deserialize_schema(value)
    raise TypeError(f"JSON API type error {value}")


def _deserialize_properties(obj: dict[str, Any]) -> dict[str, JsonSchema]:
    value = obj.get("properties", dict[str, Any]())
    return {key: deserialize_schema(schema) for key, schema in value.items()}


def _deserialize_additional(obj: dict[str, Any]) -> bool | JsonSchema | None:
    value = obj.get("additionalProperties")
    if value is None:
        return None
    if value is False:
        return value
    if isinstance(value, dict):
        return deserialize_schema(value)
    raise TypeError(f"JSON API type error {value}")


def _deserialize_one_of(obj: dict[str, Any]) -> list[JsonSchema]:
    value = obj.get("oneOf", list[JsonSchema]())
    return [deserialize_schema(one_of) for one_of in value]


def _serialize_properties(schema: JsonSchema) -> dict[str, Any]:
    return {key: serialize_schema(value) for key, value in schema.properties.items()}


def _serialize_additional(schema: JsonSchema) -> bool | dict[str, Any] | None:
    properties = schema.additional_properties
    if properties is None:
        return None
    if isinstance(properties, bool):
        return properties
    return serialize_schema(properties)


def _serialize_one_of(schema: JsonSchema) -> list[dict[str, Any]]:
    return [serialize_schema(one_of) for one_of in schema.one_of]
