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

"""Helper module to create a function calling an entrypoint."""

from typing import Callable

from ..client.abstract_client import AbstractClient
from . import function_docstring, function_params
from .entrypoint import Entrypoint


def build_function(
    client: AbstractClient,
    entrypoint: Entrypoint
) -> Callable:
    """Create a function calling the given entrypoint on the given client.

    If the entrypoint params are not a oneOf and have properties, the function
    will have these properties as keyword arguments.

    The function return type is defined by the content of the field 'result' of
    the reply that is returned without any processing.

    Example: Entrypoint(
        title='do-stuff',
        params: [{a: int, b: str}],
        returns: {c: float}
    )
    gives def function(a: int, b: str) -> Any

    Optional arguments will be defaulted to None.

    :param client: Client connected to the renderer
    :type client: AbstractClient
    :param entrypoint: Entrypoint to call in the function.
    :type entrypoint: Entrypoint
    :return: Raw entrypoint return value
    :rtype: Callable
    """
    code = _get_function_code(entrypoint)
    context = {
        '_get_result': lambda params: _get_result(client, entrypoint, params)
    }
    exec(code, context)
    return context['_function']


_PATTERN = '''from typing import Any, Union

def _function(self{params}) -> Any:
    """{docstring}"""
    return _get_result(locals())'''


def _get_result(client: AbstractClient, entrypoint: Entrypoint, params: dict):
    return client.request(
        method=entrypoint.name,
        params=_get_params(entrypoint, params)
    )


def _get_params(entrypoint: Entrypoint, params: dict):
    return _unpack_params(
        entrypoint=entrypoint,
        params=_filter_params(params)
    )


def _unpack_params(entrypoint: Entrypoint, params: dict):
    if not params:
        return None
    if entrypoint.params.unpacked:
        return params
    if len(params) != 1 or 'params' not in params:
        raise ValueError(f'Invalid packed params: {params}')
    return params['params']


def _filter_params(params: dict):
    return {
        key: value
        for key, value in params.items()
        if key != 'self' and value is not None
    }


def _get_function_code(entrypoint: Entrypoint):
    params = function_params.from_entrypoint(entrypoint)
    if params:
        params = ', ' + params
    return _PATTERN.format(
        params=params,
        docstring=function_docstring.from_entrypoint(entrypoint)
    )
