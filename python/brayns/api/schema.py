# Copyright 2015-2024 Blue Brain Project/EPFL
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
from typing import Iterable, List, Union

from . import typename


@dataclass
class Schema:
    """Contain all information about params or result schema.

    :param typename: Entrypoint Python type name (i.e. 'None', 'bool', etc)
    :type typename: str
    :param name: Schema label (name if a property)
    :type name: str
    :param description: Schema description
    :type description: str
    :param required: Check wether the params / result is required
    :type required: bool
    :param read_only: Cannot be used as params if True
    :type read_only: bool
    :param write_only: Cannot be used as result if True
    :type write_only: bool
    :param properties: Schema properties if dict (sorted by required and name)
    :type properties: List[Schema]
    :param one_of: Schema is a Union of these schemas if not empty
    :type one_of: List[Schema]
    :param items: Items schema if list
    :type items: Schema
    :param additional_properties: Items schema if hash (variable length dict)
    :type additional_properties: Schema
    """

    typename: str = 'None'
    name: str = ''
    description: str = ''
    required: bool = False
    read_only: bool = False
    write_only: bool = False
    properties: List['Schema'] = field(default_factory=list)
    one_of: List['Schema'] = field(default_factory=list)
    items: Union['Schema', None] = None
    additional_properties: Union['Schema', None] = None

    @staticmethod
    def from_dict(schema: dict, name: str = '',  required: bool = False):
        """Convert a parsed JSON schema to a Schema instance.

        :param schema: Parsed JSON schema
        :type schema: dict
        :param name: Name if the schema is a property, defaults to ''
        :type name: str, optional
        :param required: Check if the schema is required, defaults to False
        :type required: bool, optional
        :return: Schema with info extracted
        :rtype: Schema
        """
        return Schema(
            typename=typename.from_schema(schema),
            name=name,
            description=_get_description(schema),
            required=required,
            read_only=_get_read_only(schema),
            write_only=_get_write_only(schema),
            properties=_get_properties(schema),
            one_of=_get_oneof(schema, required),
            items=_get_items(schema),
            additional_properties=_get_additional_properties(schema)
        )


def _get_description(schema: dict):
    return schema.get('description', '')


def _get_read_only(schema: dict):
    return schema.get('readOnly', False)


def _get_write_only(schema: dict):
    return schema.get('writeOnly', False)


def _get_properties(schema: dict):
    required = set(schema.get('required', []))
    return _get_sorted_properties(
        Schema.from_dict(
            schema=child,
            name=name,
            required=name in required
        )
        for name, child in schema.get('properties', {}).items()
    )


def _get_sorted_properties(schemas: Iterable[Schema]):
    return sorted(
        schemas,
        key=lambda schema: (not schema.required, schema.name)
    )


def _get_oneof(schema: dict, required: bool = False):
    return [
        Schema.from_dict(
            schema=one_of,
            name=one_of.get('title', ''),
            required=required
        )
        for one_of in schema.get('oneOf', [])
    ]


def _get_items(schema: dict):
    items = schema.get('items', {})
    return Schema.from_dict(items, name='items') if items else None


def _get_additional_properties(schema: dict):
    items = schema.get('additionalProperties', False)
    return Schema.from_dict(items, 'items') if items else None
