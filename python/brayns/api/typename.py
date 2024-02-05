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

"""Helper module to get the Python typename from a JSON schema."""

from typing import List


def from_schema(schema: dict) -> str:
    """Get the Python typename from entrypoint JSON schema.

    :param schema: Entrypoint schema.
    :type schema: dict
    :return: Python typename (integer -> 'int', string -> 'str', etc.)
    :rtype: str
    """
    typename = schema.get('type')
    if typename is not None:
        return _TYPES[typename]
    one_of = schema.get('oneOf', [])
    if one_of:
        return _from_one_of(one_of)
    return 'Any'


_TYPES = {
    'null': 'None',
    'boolean': 'bool',
    'integer': 'int',
    'number': 'float',
    'string': 'str',
    'array': 'list',
    'object': 'dict'
}


def _from_one_of(one_of: List[dict]) -> str:
    typenames = {
        from_schema(schema)
        for schema in one_of
    }
    if len(typenames) == 1:
        return next(iter(typenames))
    return f'Union[{", ".join(sorted(typenames))}]'
