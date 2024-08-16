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
from brayns.utils.parsing import check_type, get, try_get


@dataclass
class Object:
    id: int
    type: str
    user_data: Any


def deserialize_object(message: dict[str, Any]) -> Object:
    return Object(
        id=get(message, "id", int),
        type=get(message, "type", str),
        user_data=try_get(message, "userData", Any, None),
    )


async def get_all_objects(connection: Connection) -> list[Object]:
    result = await connection.get_result("getAllObjects")
    check_type(result, dict[str, Any])
    objects = get(result, "objects", list[dict[str, Any]])
    return [deserialize_object(item) for item in objects]


async def get_object(connection: Connection, id: int) -> Object:
    result = await connection.get_result("getObject", {"id": id})
    check_type(result, dict[str, Any])
    return deserialize_object(result)


async def update_object(connection: Connection, id: int, user_data: Any) -> None:
    properties = {"userData": user_data}
    await connection.get_result("updateObject", {"id": id, "properties": properties})


async def remove_objects(connection: Connection, ids: list[int]) -> None:
    await connection.get_result("removeObjects", {"ids": ids})


async def clear_objects(connection: Connection) -> None:
    await connection.get_result("clearObjects")


async def create_empty_object(connection: Connection) -> int:
    result = await connection.get_result("createEmptyObject")
    check_type(result, dict[str, Any])
    return get(result, "id", int)


async def create_specific_object(connection: Connection, typename: str, params: dict[str, Any]) -> int:
    method = "create" + typename
    result = await connection.get_result(method, params)
    check_type(result, dict[str, Any])
    return get(result, "id", int)


async def create_composed_object(
    connection: Connection, typename: str, base: dict[str, Any] | None, derived: dict[str, Any] | None
) -> int:
    params = {"base": base, "derived": derived}
    return await create_specific_object(connection, typename, params)


async def get_specific_object(connection: Connection, typename: str, id: int) -> dict[str, Any]:
    method = "get" + typename
    params = {"id": id}
    result = await connection.get_result(method, params)
    check_type(result, dict[str, Any])
    return result


async def update_specific_object(connection: Connection, typename: str, id: int, properties: dict[str, Any]) -> None:
    method = "update" + typename
    params = {"id": id, "properties": properties}
    await connection.get_result(method, params)
