# Copyright (c) 2015-2022 EPFL/Blue Brain Project
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

from .json_schema import JsonSchema


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
    :param params: Schema of input if any, otherwise None.
    :type params: JsonSchema | None
    :param result: Schema of output if any, otherwise None.
    :type result: JsonSchema | None
    """

    method: str
    description: str
    plugin: str
    asynchronous: bool
    params: JsonSchema | None
    result: JsonSchema | None
