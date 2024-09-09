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

from collections.abc import Iterable
from dataclasses import dataclass
from typing import Any, NamedTuple

from brayns.network.connection import Connection
from brayns.utils.parsing import check_type, get, try_get


class Object(NamedTuple):
    id: int


@dataclass
class ObjectInfo:
    id: int
    type: str
    user_data: Any


def deserialize_object_info(message: dict[str, Any]) -> ObjectInfo:
    return ObjectInfo(
        id=get(message, "id", int),
        type=get(message, "type", str),
        user_data=try_get(message, "userData", Any, None),
    )


async def get_all_objects(connection: Connection) -> list[ObjectInfo]:
    result = await connection.get_result("getAllObjects")
    check_type(result, dict[str, Any])
    objects = get(result, "objects", list[dict[str, Any]])
    return [deserialize_object_info(item) for item in objects]


async def get_object(connection: Connection, object: Object) -> ObjectInfo:
    result = await connection.get_result("getObject", {"id": object.id})
    check_type(result, dict[str, Any])
    return deserialize_object_info(result)


async def update_object(connection: Connection, object: Object, user_data: Any) -> None:
    settings = {"userData": user_data}
    await connection.get_result("updateObject", {"id": object.id, "settings": settings})


async def remove_objects(connection: Connection, objects: Iterable[Object]) -> None:
    await connection.get_result("removeObjects", {"ids": [object.id for object in objects]})


async def clear_objects(connection: Connection) -> None:
    await connection.get_result("clearObjects")


async def create_specific_object(
    connection: Connection, typename: str, params: dict[str, Any] | None = None, binary: bytes = b""
) -> Object:
    method = "create" + typename
    result = await connection.get_result(method, params, binary)
    check_type(result, dict[str, Any])
    id = get(result, "id", int)
    return Object(id)


async def create_composed_object(
    connection: Connection,
    typename: str,
    base: dict[str, Any] | None = None,
    derived: dict[str, Any] | None = None,
    binary: bytes = b"",
) -> Object:
    params = {}

    if base is not None:
        params["base"] = base

    if derived is not None:
        params["derived"] = derived

    return await create_specific_object(connection, typename, params, binary)


async def get_specific_object(connection: Connection, typename: str, object: Object) -> dict[str, Any]:
    method = "get" + typename
    params = {"id": object.id}
    result = await connection.get_result(method, params)
    check_type(result, dict[str, Any])
    return result


async def update_specific_object(
    connection: Connection, typename: str, object: Object, settings: dict[str, Any]
) -> None:
    method = "update" + typename
    params = {"id": object.id, "settings": settings}
    await connection.get_result(method, params)


class EmptyObject(Object): ...


async def create_empty_object(connection: Connection) -> EmptyObject:
    object = await create_specific_object(connection, "EmptyObject", None)
    return EmptyObject(object.id)
