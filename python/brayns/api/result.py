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

import copy
from typing import List, Union

from .schema import Schema


class Result:

    @staticmethod
    def from_dict(schema: dict):
        if not schema:
            return Result()
        return Result.from_schema(Schema.from_dict(schema))

    @staticmethod
    def from_schema(schema: Schema):
        if schema.write_only:
            return Result()
        return Result(_format_result(schema))

    def __init__(self, schema: Union[Schema, None] = None):
        self._schema = schema

    def __bool__(self):
        return self._schema is not None

    @property
    def schema(self):
        if self._schema is None:
            raise ValueError('This entrypoint has no result')
        return self._schema


def _format_results(schemas: List[Schema]):
    return [
        _format_result(schema)
        for schema in schemas
        if not schema.write_only
    ]


def _format_result(schema: Schema):
    result = copy.deepcopy(schema)
    result.properties = _format_results(result.properties)
    result.one_of = _format_results(result.one_of)
    if result.items is not None:
        result.items = _format_result(result.items)
    if result.additional_properties is not None:
        result.additional_properties = _format_result(
            result.additional_properties
        )
    return result
