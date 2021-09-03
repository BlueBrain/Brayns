from typing import Any

from . import function_args
from . import client_interface

from .entrypoint import Entrypoint
from .client_interface import Client


def build_function(client: Client, entrypoint: Entrypoint) -> Any:
    method = entrypoint.name
    code = _get_function_code(entrypoint)
    context = {
        '_get_result': lambda params: client_interface.get_result(
            client,
            method,
            params
        )
    }
    exec(code, context)
    return context['_function']


_PATTERN = '''
from typing import Any, Union

def _function(self{args}) -> Any:
    """{docstring}"""
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
    return f'{entrypoint.description}.{_format_args_description(entrypoint)}'


def _format_args_description(entrypoint: Entrypoint) -> str:
    args = function_args.get_descriptions(entrypoint.params)
    if not args:
        return ''
    separator = '\n    '
    return f'\n\n    Keyword arguments:\n    {separator.join(args)}\n    '
