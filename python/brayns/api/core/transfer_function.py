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

from dataclasses import dataclass, field
from typing import Any

from brayns.network.connection import Connection
from brayns.utils.box import Box1
from brayns.utils.color import Color4
from brayns.utils.parsing import deserialize_box1, get, serialize_box1

from .objects import Object, create_composed_object, get_specific_object, update_specific_object


class TransferFunction(Object): ...


async def create_transfer_function(connection: Connection, typename: str, derived: dict[str, Any]) -> TransferFunction:
    object = await create_composed_object(connection, typename, None, derived)
    return TransferFunction(object.id)


class LinearTransferFunction(TransferFunction): ...


@dataclass
class LinearTransferFunctionSettings:
    scalar_range: Box1 = Box1(0, 1)
    colors: list[Color4] = field(default_factory=lambda: [Color4(0, 0, 0), Color4(1, 1, 1)])


def serialize_linear_transfer_function_settings(settings: LinearTransferFunctionSettings) -> dict[str, Any]:
    return {
        "scalarRange": serialize_box1(settings.scalar_range),
        "colors": [list(color) for color in settings.colors],
    }


def deserialize_linear_transfer_function_settings(message: dict[str, Any]) -> LinearTransferFunctionSettings:
    return LinearTransferFunctionSettings(
        scalar_range=deserialize_box1(get(message, "scalarRange", dict[str, Any])),
        colors=[Color4(*color) for color in get(message, "colors", list[list[float]])],
    )


async def create_linear_transfer_function(
    connection: Connection, settings: LinearTransferFunctionSettings
) -> LinearTransferFunction:
    derived = serialize_linear_transfer_function_settings(settings)
    object = await create_transfer_function(connection, "LinearTransferFunction", derived)
    return LinearTransferFunction(object.id)


async def get_linear_transfer_function(
    connection: Connection, transfer_function: LinearTransferFunction
) -> LinearTransferFunctionSettings:
    result = await get_specific_object(connection, "LinearTransferFunction", transfer_function)
    return deserialize_linear_transfer_function_settings(result)


async def update_linear_transfer_function(
    connection: Connection, transfer_function: LinearTransferFunction, settings: LinearTransferFunctionSettings
) -> None:
    params = serialize_linear_transfer_function_settings(settings)
    await update_specific_object(connection, "LinearTransferFunction", transfer_function, params)
