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
from typing import Any, Optional

from brayns.core.api.json_type import JsonType


@dataclass
class JsonSchema:

    title: str = ''
    description: str = ''
    type: JsonType = JsonType.UNDEFINED
    read_only: bool = False
    write_only: bool = False
    default_value: Any = None
    minimum: Optional[float] = None
    maximum: Optional[float] = None
    items: Optional['JsonSchema'] = None
    min_items: Optional[int] = None
    max_items: Optional[int] = None
    properties: dict[str, 'JsonSchema'] = field(default_factory=dict)
    required: list[str] = field(default_factory=list)
    additional_properties: Optional['JsonSchema'] = None
    one_of: list['JsonSchema'] = field(default_factory=list)
    enums: list[Any] = field(default_factory=list)

    @staticmethod
    def deserialize(message: dict) -> 'JsonSchema':
        return JsonSchema(
            title=message.get('title', ''),
            description=message.get('description', ''),
            type=JsonType(message.get('type', JsonType.UNDEFINED)),
            read_only=message.get('readOnly', False),
            write_only=message.get('writeOnly', False),
            default_value=message.get('default'),
            minimum=message.get('minimum'),
            maximum=message.get('maximum'),
            items=_parse_items(message),
            min_items=message.get('minItems'),
            max_items=message.get('maxItems'),
            properties=_parse_properties(message),
            required=message.get('required', []),
            additional_properties=_parse_additional_properties(message),
            one_of=_parse_one_of(message),
            enums=message.get('enums', [])
        )

    @classmethod
    @property
    def wildcard(cls) -> 'JsonSchema':
        return JsonSchema()

    def is_one_of(self) -> bool:
        return bool(self.one_of)

    def is_enum(self) -> bool:
        return bool(self.enums)

    def is_wildcard(self) -> bool:
        return not self.is_one_of() and self.type is JsonType.UNDEFINED


def _parse_items(message: dict) -> JsonSchema:
    value = message.get('items')
    if value is None:
        return None
    return JsonSchema.deserialize(value)


def _parse_properties(message: dict) -> dict[str, JsonSchema]:
    return {
        key: JsonSchema.deserialize(value)
        for key, value in message.get('properties', {}).items()
    }


def _parse_additional_properties(message: dict) -> Optional[JsonSchema]:
    if message.get('type') != JsonType.OBJECT.value:
        return None
    value = message.get('additionalProperties')
    if value is None:
        return JsonSchema.wildcard
    if not value:
        return None
    return JsonSchema.deserialize(value)


def _parse_one_of(message: dict) -> Optional[JsonSchema]:
    return [
        JsonSchema.deserialize(one_of)
        for one_of in message.get('oneOf', [])
    ]
