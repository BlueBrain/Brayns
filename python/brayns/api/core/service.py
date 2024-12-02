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
from brayns.network.json_rpc import JsonRpcId
from brayns.utils.parsing import deserialize


@dataclass
class Version:
    major: int
    minor: int
    patch: int
    pre_release: int
    tag: str


async def get_version(connection: Connection) -> Version:
    result = await connection.get_result("getVersion")
    return deserialize(result, Version)


async def get_tasks(connection: Connection) -> list[JsonRpcId]:
    result = await connection.get_result("getTasks")
    return deserialize(result, list[JsonRpcId])


@dataclass
class TaskOperation:
    description: str
    completion: float


async def get_task(connection: Connection, task_id: JsonRpcId) -> TaskOperation:
    result = await connection.get_result("getTask", {"task": task_id})
    return deserialize(result, TaskOperation)


async def cancel_task(connection: Connection, task_id: JsonRpcId) -> None:
    await connection.get_result("cancelTask", {"task": task_id})


async def get_methods(connection: Connection) -> list[str]:
    result = await connection.get_result("getMethods")
    return deserialize(result, list[str])


@dataclass
class Endpoint:
    method: str
    description: str
    params: dict[str, Any]
    result: dict[str, Any]


async def get_endpoint(connection: Connection, method: str) -> Endpoint:
    result = await connection.get_result("getSchema", {"method": method})
    return deserialize(result, Endpoint)


async def stop_service(connection: Connection) -> None:
    await connection.get_result("stop")
