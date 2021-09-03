from typing import List

import types

from . import entrypoint
from . import function_builder

from .client_interface import Client
from .entrypoint import Entrypoint


def build_api(client: Client) -> None:
    for schema in _get_all_schemas(client):
        _add_method(client, entrypoint.from_schema(schema))


def _get_all_schemas(client: Client) -> List[str]:
    return [
        client.get('schema', {'endpoint': endpoint})
        for endpoint in client.get('registry')
    ]


def _add_method(client: Client, entrypoint: Entrypoint) -> None:
    setattr(
        client,
        entrypoint.name.replace('-', '_'),
        _get_method(client, entrypoint)
    )


def _get_method(client: Client, entrypoint: Entrypoint) -> types.MethodType:
    return types.MethodType(
        function_builder.build_function(client, entrypoint),
        client
    )
