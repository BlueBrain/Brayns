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

from collections.abc import Callable
from dataclasses import dataclass
from typing import Any, Generic, TypeVar

from brayns.network.connection import Connection, Response
from brayns.network.json_rpc import get


@dataclass
class Version:
    major: int
    minor: int
    patch: int
    pre_release: int
    tag: str


async def get_version(connection: Connection) -> Version:
    result = await connection.get_result("getVersion")

    return Version(
        major=get(result, "major", int),
        minor=get(result, "minor", int),
        patch=get(result, "patch", int),
        pre_release=get(result, "preRelease", int),
        tag=get(result, "tag", str),
    )


async def get_methods(connection: Connection) -> list[str]:
    result = await connection.get_result("getMethods")

    return get(result, "methods", list[str])


@dataclass
class Endpoint:
    method: str
    description: str
    params_schema: dict[str, Any]
    result_schema: dict[str, Any]
    asynchronous: bool


async def get_endpoint(connection: Connection, method: str) -> Endpoint:
    result = await connection.get_result("getSchema", {"method": method})

    return Endpoint(
        method=get(result, "method", str),
        description=get(result, "description", str),
        params_schema=get(result, "params", dict[str, Any]),
        result_schema=get(result, "result", dict[str, Any]),
        asynchronous=get(result, "async", bool),
    )


class Task:
    def __init__(self, id: int) -> None:
        self._id = id

    @property
    def id(self) -> int:
        return self._id


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

    @property
    def done(self) -> bool:
        index = self.current_operation.index
        completion = self.current_operation.completion
        return index == self.operation_count - 1 and completion == 1.0


def deserialize_task(message: dict[str, Any]) -> TaskInfo:
    operation = get(message, "currentOperation", dict[str, Any])

    return TaskInfo(
        id=get(message, "id", int),
        operation_count=get(message, "operationCount", int),
        current_operation=TaskOperation(
            description=get(operation, "description", str),
            index=get(operation, "index", int),
            completion=get(operation, "completion", float),
        ),
    )


async def get_tasks(connection: Connection) -> list[TaskInfo]:
    result = await connection.get_result("getTasks")
    tasks = get(result, "tasks", list[dict[str, Any]])
    return [deserialize_task(task) for task in tasks]


async def get_task(connection: Connection, task: Task) -> TaskInfo:
    result = await connection.get_result("getTask", {"taskId": task.id})
    return deserialize_task(result)


async def cancel_task(connection: Connection, task: Task) -> None:
    await connection.get_result("cancelTask", {"taskId": task.id})


async def cancel_all_tasks(connection: Connection) -> None:
    await connection.get_result("cancelAllTasks")


async def get_task_result(connection: Connection, task: Task) -> Response:
    return await connection.request("getTaskResult", {"taskId": task.id})


async def stop_service(connection: Connection) -> None:
    await connection.get_result("stop")


T = TypeVar("T")


class TaskWrapper(Generic[T]):
    def __init__(self, task: Task, parser: Callable[[Response], T]) -> None:
        self._task = task
        self._parser = parser

    @property
    def id(self) -> int:
        return self._task.id

    async def get_status(self, connection: Connection) -> TaskInfo:
        return await get_task(connection, self._task)

    async def is_done(self, connection: Connection) -> bool:
        status = await self.get_status(connection)
        return status.done

    async def cancel(self, connection: Connection) -> None:
        await cancel_task(connection, self._task)

    async def wait(self, connection: Connection) -> T:
        result = await get_task_result(connection, self._task)
        return self._parser(result)
