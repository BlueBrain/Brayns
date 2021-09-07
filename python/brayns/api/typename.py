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

def from_schema(schema: dict) -> str:
    typename = schema.get('type')
    if typename is not None:
        return _TYPES[typename].__name__
    return _from_oneof(schema)


_TYPES = {
    'boolean': bool,
    'integer': int,
    'number': float,
    'string': str,
    'array': list,
    'object': dict
}


def _from_oneof(schema: dict) -> str:
    typenames = ', '.join(
        from_schema(oneof)
        for oneof in schema['oneOf']
        if oneof.get('type') != 'null'
    )
    return f'Union[{typenames}]'
