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

"""Brayns API builder.

Use Brayns JSON-RPC entrypoints schemas to add Python methods to the client.
"""

import types

from ..client.abstract_client import AbstractClient
from . import entrypoint_loader, function_builder
from .entrypoint import Entrypoint


def build_api(client: AbstractClient) -> None:
    """Build Brayns Python API using the provided client instance.

    Retreive all entrypoint schemas using the client (must be connected), use it
    to build Python functions calling the corresponding JSON-RPC entrypoint and
    bind these functions as client methods.

    :param client: client instance connected to the renderer
    :type client: AbstractClient
    """
    for entrypoint in entrypoint_loader.load_all_entrypoints(client):
        _add_method(client, entrypoint)


def _add_method(client: AbstractClient, entrypoint: Entrypoint) -> None:
    setattr(
        client,
        entrypoint.name.replace('-', '_'),
        _create_method(client, entrypoint)
    )


def _create_method(
    client: AbstractClient,
    entrypoint: Entrypoint
) -> types.MethodType:
    return types.MethodType(
        function_builder.build_function(client, entrypoint),
        client
    )
