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

"""Brayns API builder.

Use Brayns JSON-RPC entrypoints schemas to add Python methods to the client.
"""

import types
from typing import List

from brayns.client.abstract_client import AbstractClient

from . import function_builder
from .entrypoint import Entrypoint


def build_api(client: AbstractClient) -> None:
    """Build Brayns Python API using the provided client instance.

    Retreive all entrypoint schemas using the client (must be connected), use it
    to build Python functions calling the corresponding JSON-RPC entrypoint and
    bind these functions as client methods.

    :param client: client instance connected to the renderer
    :type client: AbstractClient
    """
    for schema in _get_all_schemas(client):
        _add_method(client, Entrypoint.from_schema(schema))


def _get_all_schemas(client: AbstractClient) -> List[str]:
    return [
        client.request('schema', {'endpoint': endpoint})
        for endpoint in client.request('registry')
    ]


def _add_method(client: AbstractClient, entrypoint: Entrypoint) -> None:
    setattr(
        client,
        entrypoint.name.replace('-', '_'),
        _get_method(client, entrypoint)
    )


def _get_method(
    client: AbstractClient,
    entrypoint: Entrypoint
) -> types.MethodType:
    return types.MethodType(
        function_builder.build_function(client, entrypoint),
        client
    )
