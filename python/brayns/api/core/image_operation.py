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
from typing import TypedDict, Unpack

from brayns.network.connection import Connection
from brayns.utils.composing import serialize
from brayns.utils.parsing import deserialize

from .objects import CreateObjectParams, Object, create_specific_object, get_specific_object, update_specific_object


class ImageOperation(Object): ...


class ToneMapper(ImageOperation): ...


@dataclass
class GetToneMapperResult:
    exposure: float
    contrast: float
    shoulder: float
    mid_in: float
    mid_out: float
    hdr_max: float
    aces_color: bool


class UpdateToneMapperParams(TypedDict, total=False):
    exposure: float
    contrast: float
    shoulder: float
    mid_in: float
    mid_out: float
    hdr_max: float
    aces_color: bool


class CreateToneMapperParams(CreateObjectParams, UpdateToneMapperParams): ...


async def create_tone_mapper(connection: Connection, **settings: Unpack[CreateToneMapperParams]) -> ToneMapper:
    object = await create_specific_object(connection, "ToneMapper", serialize(settings))
    return ToneMapper(object.id)


async def get_tone_mapper(connection: Connection, tone_mapper: ToneMapper) -> GetToneMapperResult:
    result = await get_specific_object(connection, "ToneMapper", tone_mapper)
    return deserialize(result, GetToneMapperResult)


async def update_tone_mapper(
    connection: Connection, tone_mapper: ToneMapper, **settings: Unpack[UpdateToneMapperParams]
) -> None:
    await update_specific_object(connection, "ToneMapper", tone_mapper, serialize(settings))
