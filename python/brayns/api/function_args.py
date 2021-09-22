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

from typing import List

from .params import Params
from .property import Property


def get_declarations(params: Params) -> List[str]:
    return [
        get_declaration(property)
        for property in _get_properties(params)
    ]


def get_declaration(property: Property) -> str:
    name = property.name
    typename = property.typename
    default = '' if property.required else ' = None'
    return f'{name}: {typename}{default}'


def get_descriptions(params: Params) -> List[str]:
    return [
        get_description(property)
        for property in _get_properties(params)
    ]


def get_description(property: Property) -> str:
    return _PATTERN.format(
        name=property.name,
        typename=property.typename,
        default='' if property.required else ', defaults to None',
        optional='' if property.required else ', optional',
        description=property.description
    )


_PATTERN = '''
    :param {name}: {description}{default}
    :type {name}: {typename}{optional}
'''[1:-1]


def _get_properties(params: Params) -> List[Property]:
    return [
        property
        for property in _get_sorted_properties(params)
        if not property.read_only
    ]


def _get_sorted_properties(params: Params) -> List[Property]:
    return sorted(
        params.properties,
        key=lambda property: (not property.required, property.name)
    )
