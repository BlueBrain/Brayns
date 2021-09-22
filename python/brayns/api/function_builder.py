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

from typing import Any, Callable

from brayns.client.abstract_client import AbstractClient

from . import function_args
from .entrypoint import Entrypoint


def build_function(
    client: AbstractClient,
    entrypoint: Entrypoint
) -> Callable:
    method = entrypoint.name
    code = _get_function_code(entrypoint)
    context = {
        '_get_result': lambda params: _get_result(client, method, params)
    }
    exec(code, context)
    return context['_function']


def _get_result(client: AbstractClient, method: str, params: dict) -> Any:
    args = {
        key: value
        for key, value in params.items()
        if key != 'self' and value is not None
    }
    return client.request(method, args if args else None)


_PATTERN = '''
from typing import Any, Union

def _function(self{args}) -> Any:
    """{docstring}."""
    return _get_result(locals())
'''.strip()


def _get_function_code(entrypoint: Entrypoint) -> str:
    return _PATTERN.format(
        args=_format_args(entrypoint),
        docstring=_get_function_docstring(entrypoint)
    )


def _format_args(entrypoint: Entrypoint) -> str:
    args = function_args.get_declarations(entrypoint.params)
    return (', ' + ', '.join(args)) if args else ''


def _get_function_docstring(entrypoint: Entrypoint) -> str:
    return f'{entrypoint.description}.{_format_params(entrypoint)}'


def _format_params(entrypoint: Entrypoint) -> str:
    args = function_args.get_descriptions(entrypoint.params)
    if not args:
        return ''
    separator = '\n'
    return f'\n\n{separator.join(args)}\n    '
