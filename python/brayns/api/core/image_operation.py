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
from typing import Any, NewType

from brayns.network.connection import Connection
from brayns.utils.parsing import get

from .objects import create_composed_object, get_specific_object, update_specific_object

ImageOperationId = NewType("ImageOperationId", int)


async def create_image_operation(connection: Connection, typename: str, derived: dict[str, Any]) -> ImageOperationId:
    id = await create_composed_object(connection, typename, None, derived)
    return ImageOperationId(id)


@dataclass
class ToneMapperSettings:
    exposure: float = 1.0
    contrast: float = 1.6773
    shoulder: float = 0.9714
    mid_in: float = 0.18
    mid_out: float = 0.18
    hdr_max: float = 11.0785
    aces_color: bool = True


def serialize_tone_mapper_settings(settings: ToneMapperSettings) -> dict[str, Any]:
    return {
        "exposure": settings.exposure,
        "contrast": settings.contrast,
        "shoulder": settings.shoulder,
        "midIn": settings.mid_in,
        "midOut": settings.mid_out,
        "hdrMax": settings.hdr_max,
        "acesColor": settings.aces_color,
    }


def deserialize_tone_mapper_settings(message: dict[str, Any]) -> ToneMapperSettings:
    return ToneMapperSettings(
        exposure=get(message, "exposure", float),
        contrast=get(message, "contrast", float),
        shoulder=get(message, "shoulder", float),
        mid_in=get(message, "midIn", float),
        mid_out=get(message, "midOut", float),
        hdr_max=get(message, "hdrMax", float),
        aces_color=get(message, "acesColor", float),
    )


async def create_tone_mapper(connection: Connection, settings: ToneMapperSettings) -> ImageOperationId:
    derived = serialize_tone_mapper_settings(settings)
    return await create_image_operation(connection, "ToneMapper", derived)


async def get_tone_mapper(connection: Connection, id: ImageOperationId) -> ToneMapperSettings:
    result = await get_specific_object(connection, "ToneMapper", id)
    return deserialize_tone_mapper_settings(result)


async def update_tone_mapper(connection: Connection, id: ImageOperationId, settings: ToneMapperSettings) -> None:
    properties = serialize_tone_mapper_settings(settings)
    await update_specific_object(connection, "ToneMapper", id, properties)
