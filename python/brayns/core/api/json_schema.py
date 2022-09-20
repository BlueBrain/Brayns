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

from .json_type import JsonType


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
