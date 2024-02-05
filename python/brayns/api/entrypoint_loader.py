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

"""Helper module to get all entrypoints registered in the renderer."""

from typing import List

from ..client.abstract_client import AbstractClient
from .entrypoint import Entrypoint


def load_all_entrypoints(client: AbstractClient) -> List[Entrypoint]:
    """Fetch all entrypoints using the client and return the list.

    :param client: Brayns client connected to the renderer
    :type client: AbstractClient
    :return: List of all entrypoints registered in the renderer
    :rtype: List[Entrypoint]
    """
    return [
        Entrypoint.from_schema(schema)
        for schema in _get_all_schemas(client)
    ]


def _get_all_schemas(client: AbstractClient) -> List[str]:
    return [
        client.request('schema', {'endpoint': endpoint})
        for endpoint in client.request('registry')
    ]
