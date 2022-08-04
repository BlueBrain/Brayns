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

from brayns.core.api.json_schema import JsonSchema
from brayns.core.api.json_type import JsonType


def serialize_json_schema(schema: JsonSchema) -> dict[str, Any]:
    result = {}
    if schema.title:
        result['title'] = schema.title
    if schema.description:
        result['description'] = schema.description
    if schema.type is not JsonType.UNDEFINED:
        result['type'] = schema.type.value
    if schema.read_only:
        result['readOnly'] = schema.read_only
    if schema.write_only:
        result['writeOnly'] = schema.write_only
    if schema.default is not None:
        result['default'] = schema.default
    if schema.minimum is not None:
        result['minimum'] = schema.minimum
    if schema.maximum is not None:
        result['maximum'] = schema.maximum
    if schema.items is not None:
        result['items'] = serialize_json_schema(schema.items)
    if schema.min_items is not None:
        result['minItems'] = schema.min_items
    if schema.max_items is not None:
        result['maxItems'] = schema.max_items
    if schema.properties:
        result['properties'] = _serialize_properties(schema)
    if schema.required:
        result['required'] = schema.required
    if schema.additional_properties is not None:
        result['additionalProperties'] = _serialize_additional(schema)
    if schema.one_of:
        result['oneOf'] = _serialize_one_of(schema)
    if schema.enum:
        result['enum'] = schema.enum
    return result


def _serialize_properties(schema: JsonSchema) -> dict[str, dict]:
    return {
        key: serialize_json_schema(value)
        for key, value in schema.properties.items()
    }


def _serialize_additional(schema: JsonSchema) -> bool | dict[str, Any] | None:
    properties = schema.additional_properties
    if properties is None:
        return None
    if isinstance(properties, bool):
        return properties
    return serialize_json_schema(properties)


def _serialize_one_of(schema: JsonSchema) -> list[dict]:
    return [
        serialize_json_schema(one_of)
        for one_of in schema.one_of
    ]
