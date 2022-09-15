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


def serialize_schema(schema: JsonSchema) -> dict[str, Any]:
    message = dict[str, Any]()
    if schema.title:
        message['title'] = schema.title
    if schema.description:
        message['description'] = schema.description
    if schema.type is not JsonType.UNDEFINED:
        message['type'] = schema.type.value
    if schema.read_only:
        message['readOnly'] = schema.read_only
    if schema.write_only:
        message['writeOnly'] = schema.write_only
    if schema.default is not None:
        message['default'] = schema.default
    if schema.minimum is not None:
        message['minimum'] = schema.minimum
    if schema.maximum is not None:
        message['maximum'] = schema.maximum
    if schema.items is not None:
        message['items'] = serialize_schema(schema.items)
    if schema.min_items is not None:
        message['minItems'] = schema.min_items
    if schema.max_items is not None:
        message['maxItems'] = schema.max_items
    if schema.properties:
        message['properties'] = _serialize_properties(schema)
    if schema.required:
        message['required'] = schema.required
    if schema.additional_properties is not None:
        message['additionalProperties'] = _serialize_additional(schema)
    if schema.one_of:
        message['oneOf'] = _serialize_one_of(schema)
    if schema.enum:
        message['enum'] = schema.enum
    return message


def _serialize_properties(schema: JsonSchema) -> dict[str, Any]:
    return {
        key: serialize_schema(value)
        for key, value in schema.properties.items()
    }


def _serialize_additional(schema: JsonSchema) -> bool | dict[str, Any] | None:
    properties = schema.additional_properties
    if properties is None:
        return None
    if isinstance(properties, bool):
        return properties
    return serialize_schema(properties)


def _serialize_one_of(schema: JsonSchema) -> list[dict[str, Any]]:
    return [
        serialize_schema(one_of)
        for one_of in schema.one_of
    ]
