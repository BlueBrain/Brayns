# Copyright (c) 2015-2023 EPFL/Blue Brain Project
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

from __future__ import annotations

from dataclasses import dataclass
from typing import Any

from brayns.network import Instance
from brayns.utils import JsonSchema, deserialize_schema


@dataclass
class Entrypoint:
    """Entrypoint (endpoint) of the JSON-RPC API.

    Describes how a given entrypoint of the JSON-RPC API can be used.

    Available entrypoints can be queried from an instance to inspect the
    Web API of a given instance.

    :param method: JSON-RPC method (ex: 'get-camera-name').
    :type method: str
    :param description: Human readable description.
    :type description: str
    :param plugin: Name of the plugin which loads the entrypoint.
    :type plugin: str
    :param asynchronous: Check wether progress and cancellation are supported.
    :type asynchronous: bool
    :param deprecated: Indicate a removal / renaming in the next major release.
    :type deprecated: bool
    :param params: Schema of input if any, otherwise None.
    :type params: JsonSchema | None
    :param result: Schema of output if any, otherwise None.
    :type result: JsonSchema | None
    """

    method: str
    description: str
    plugin: str
    asynchronous: bool
    deprecated: bool
    params: JsonSchema | None
    result: JsonSchema | None


def get_methods(instance: Instance) -> list[str]:
    """Retreive all JSON-RPC methods from an instance.

    :param instance: Instance to query the methods.
    :type instance: Instance
    :return: List of available methods (depends on plugins loaded).
    :rtype: list[str]
    """
    return instance.request("registry")


def get_entrypoint(instance: Instance, method: str) -> Entrypoint:
    """Retreive an entrypoint using its name (JSON-RPC method).

    :param instance: Instance to query the entrypoint.
    :type instance: Instance
    :param method: JSON-RPC method name.
    :type method: str
    :return: Deserialized entrypoint.
    :rtype: Entrypoint
    """
    params = {"endpoint": method}
    result = instance.request("schema", params)
    return _deserialize_entrypoint(result)


def get_entrypoints(instance: Instance) -> list[Entrypoint]:
    """Retreive all available entrypoints from an instance.

    :param instance: Instance to query the entrypoints.
    :type instance: Instance
    :return: List of available entrypoints (depends on plugins loaded).
    :rtype: list[Entrypoint]
    """
    return [get_entrypoint(instance, method) for method in get_methods(instance)]


def _deserialize_entrypoint(message: dict[str, Any]) -> Entrypoint:
    return Entrypoint(
        method=message["title"],
        description=message["description"],
        plugin=message["plugin"],
        asynchronous=message["async"],
        deprecated=message.get("deprecated", False),
        params=_deserialize_schema(message, "params"),
        result=_deserialize_schema(message, "returns"),
    )


def _deserialize_schema(message: dict[str, Any], key: str) -> JsonSchema | None:
    value = message.get(key)
    if value is None:
        return None
    return deserialize_schema(value)
