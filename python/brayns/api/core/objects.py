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
from typing import Any, TypedDict, Unpack

from brayns.network.connection import Connection
from brayns.utils.composing import Object, serialize
from brayns.utils.parsing import deserialize


@dataclass
class ObjectSummary:
    id: int
    type: str
    tag: str


async def get_all_objects(connection: Connection) -> list[ObjectSummary]:
    result = await connection.get_result("getAllObjects")
    return deserialize(result, list[ObjectSummary])


@dataclass
class GetObjectResult:
    type: str
    tag: str
    user_data: Any


class UpdateObjectParams(TypedDict, total=False):
    tag: str
    user_data: Any


class CreateObjectParams(UpdateObjectParams): ...


async def get_object(connection: Connection, object: Object) -> GetObjectResult:
    result = await connection.get_result("getObject", {"id": object.id})
    return deserialize(result, GetObjectResult)


async def update_object(connection: Connection, object: Object, **settings: Unpack[UpdateObjectParams]) -> None:
    await connection.get_result("updateObject", {"id": object.id, "settings": serialize(settings)})


async def remove_objects(connection: Connection, objects: Iterable[Object]) -> None:
    await connection.get_result("removeObjects", {"ids": [object.id for object in objects]})


async def clear_objects(connection: Connection) -> None:
    await connection.get_result("clearObjects")


async def create_specific_object(
    connection: Connection, typename: str, params: dict[str, Any], binary: bytes = b""
) -> Object:
    method = "create" + typename
    result = await connection.get_result(method, params, binary)
    return Object(result["id"])


async def get_specific_object(connection: Connection, typename: str, object: Object) -> dict[str, Any]:
    method = "get" + typename
    params = {"id": object.id}
    return await connection.get_result(method, params)


async def update_specific_object(
    connection: Connection, typename: str, object: Object, settings: dict[str, Any]
) -> None:
    method = "update" + typename
    params = {"id": object.id, "settings": settings}
    await connection.get_result(method, params)


class EmptyObject(Object): ...


async def create_empty_object(connection: Connection, **settings: Unpack[CreateObjectParams]) -> EmptyObject:
    object = await create_specific_object(connection, "EmptyObject", serialize(settings))
    return EmptyObject(object.id)
