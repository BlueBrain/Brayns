# Copyright (c) 2015-2024 EPFL/Blue Brain Project
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

from dataclasses import dataclass
from typing import Any

from brayns.network.connection import Connection
from brayns.network.json_rpc import get


@dataclass
class Version:
    major: int
    minor: int
    patch: int
    pre_release: int
    tag: str


async def get_version(connection: Connection) -> Version:
    result = await connection.get_result("get-version")

    return Version(
        major=get(result, "major", int),
        minor=get(result, "minor", int),
        patch=get(result, "patch", int),
        pre_release=get(result, "pre_release", int),
        tag=get(result, "tag", str),
    )


async def get_methods(connection: Connection) -> list[str]:
    result = await connection.get_result("get-methods")

    return get(result, "methods", list[str])


@dataclass
class Endpoint:
    method: str
    description: str
    params_schema: dict[str, Any]
    result_schema: dict[str, Any]
    asynchronous: bool


async def get_endpoint(connection: Connection, method: str) -> Endpoint:
    result = await connection.get_result("get-schema", {"method": method})

    return Endpoint(
        method=get(result, "method", str),
        description=get(result, "description", str),
        params_schema=get(result, "params", dict[str, Any]),
        result_schema=get(result, "result", dict[str, Any]),
        asynchronous=get(result, "async", bool),
    )


@dataclass
class TaskOperation:
    description: str
    index: int
    completion: float


@dataclass
class TaskInfo:
    id: int
    operation_count: int
    current_operation: TaskOperation
