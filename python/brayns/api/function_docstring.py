# Copyright (c) 2015-2021 EPFL/Blue Brain Project
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

"""Helper module to build an entrypoint function docstring."""

from typing import List, Union

from .entrypoint import Entrypoint
from .schema import Schema


def from_entrypoint(entrypoint: Entrypoint):
    """Create a docstring using Sphinx format for the given entrypoint.

    :param entrypoint: Source of the function.
    :type entrypoint: Entrypoint
    :return: Sphinx docstring.
    :rtype: str
    """
    return _FUNCTION.format(
        name=entrypoint.name,
        description=entrypoint.description,
        params_and_result=_get_params_and_result_docstring(entrypoint)
    )


_FUNCTION = '''Auto-generated method calling entrypoint '{name}'.

    {description}.{params_and_result}
    '''

_PARAMS_AND_RESULT = '''
    {params}{result}'''

_PARAM = '''
    :param {name}: {description}{default}
    :type {name}: {typename}{optional}'''


_RETURN = '''
    :return: {description}
    :rtype: {typename}'''


def _get_params_and_result_docstring(entrypoint: Entrypoint):
    if not entrypoint.params and entrypoint.result is None:
        return ''
    return _PARAMS_AND_RESULT.format(
        params=_get_params_docstring(entrypoint.params),
        result=_get_result_docstring(entrypoint.result)
    )


def _get_params_docstring(schemas: List[Schema]):
    return ''.join(
        _get_param_docstring(schema)
        for schema in schemas
    )


def _get_param_docstring(schema: Schema):
    return _PARAM.format(
        name=schema.name,
        typename=schema.typename,
        default='' if schema.required else ', defaults to None',
        optional='' if schema.required else ', optional',
        description=schema.description or 'No available descriptions'
    )


def _get_result_docstring(schema: Union[Schema, None]):
    return _RETURN.format(
        description=_get_description(schema),
        typename=_get_typename(schema)
    )


def _get_description(schema: Union[Schema, None]):
    if schema is None:
        return 'This method has no return value'
    return schema.description or 'No descriptions available'


def _get_typename(schema: Union[Schema, None]):
    return schema.typename if schema is not None else 'None'
