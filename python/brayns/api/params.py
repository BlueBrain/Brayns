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
from dataclasses import dataclass, field
from typing import List

from .schema import Schema


@dataclass
class Params:

    schemas: List[Schema] = field(default_factory=list)
    unpacked: bool = False

    @staticmethod
    def from_dicts(schemas: List[dict]):
        if not schemas:
            return Params(
                schemas=[],
                unpacked=False
            )
        return Params.from_dict(schemas[0])

    @staticmethod
    def from_dict(schema: dict):
        return Params.from_schema(Schema.from_dict(schema))

    @staticmethod
    def from_schema(schema: Schema):
        if not schema.properties:
            return Params(
                schemas=_format_params([schema]),
                unpacked=False
            )
        return Params(
            schemas=_format_params(schema.properties),
            unpacked=True
        )

    def __bool__(self):
        return bool(self.schemas)


def _format_params(schemas: List[Schema]):
    return [
        _format_param(schema)
        for schema in schemas
        if not schema.read_only
    ]


def _format_param(schema: Schema):
    param = copy.deepcopy(schema)
    if not param.name:
        param.name = 'params'
    param.properties = _format_params(param.properties)
    param.one_of = _format_params(param.one_of)
    if param.items is not None:
        param.items = _format_param(param.items)
    if param.additional_properties is not None:
        param.additional_properties = _format_param(
            param.additional_properties
        )
    return param
