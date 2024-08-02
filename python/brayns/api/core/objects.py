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
from typing import Any, TypeVar
from brayns.network.connection import Connection
from brayns.utils.parsing import get, check_type


@dataclass
class Metadata:
    id: int
    type: str
    user_data: Any


@dataclass
class Object:
    metadata: Metadata

    @property
    def id(self) -> int:
        return self.metadata.id

    @property
    def user_data(self) -> Any:
        return self.metadata.user_data


def parse_metadata(message: dict[str, Any]) -> Metadata:
    return Metadata(
        id=get(message, "id", int),
        type=get(message, "type", str),
        user_data=get(message, "user_data", Any),
    )


def extract_metadata(result: dict[str, Any]) -> Metadata:
    return parse_metadata(result["metadata"])


T = TypeVar("T", bound=Object)


async def get_all_objects(connection: Connection) -> list[Object]:
    result = await connection.get_result("get-all-objects")
    check_type(result, dict[str, Any])
    objects = get(result, "objects", list[dict[str, Any]])
    return [Object(parse_metadata(item)) for item in objects]


async def get_object(connection: Connection, id: int) -> Object:
    result = await connection.get_result("get-object", {"id": id})
    check_type(result, dict[str, Any])
    return Object(parse_metadata(result))


async def remove_objects(connection: Connection, ids: list[int]) -> None:
    await connection.get_result("remove-objects", {"ids": ids})


async def clear_objects(connection: Connection) -> None:
    await connection.get_result("clear-objects")


async def create_object(connection: Connection, user_data: Any = None) -> Object:
    result = await connection.get_result("create-empty-object", {"user_data": user_data})
    check_type(result, dict[str, Any])
    return Object(extract_metadata(result))


T = TypeVar("T", bound=Metadata)
