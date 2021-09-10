# Copyright (c) 2021 EPFL/Blue Brain Project
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
from typing import List

from .property import Property


@dataclass
class Params:

    properties: List[Property] = field(default_factory=list)

    @staticmethod
    def from_schemas(schemas: List[dict]):
        return Params.from_schema(schemas[0]) if schemas else Params()

    @staticmethod
    def from_schema(schema: dict):
        if 'properties' in schema:
            return Params.from_properties(schema)
        if 'oneOf' in schema:
            return Params.from_oneof(schema)
        raise ValueError(f'Invalid schema: {schema!r}')

    @staticmethod
    def from_properties(params: dict):
        required = set(params.get('required', []))
        return Params([
            Property.from_schema(
                name=name,
                schema=schema,
                required=name in required
            )
            for name, schema in params['properties'].items()
        ])

    @staticmethod
    def from_oneof(schema: dict):
        return Params([
            Property.from_schema(
                name='params',
                schema=schema,
                required=True
            )
        ])
