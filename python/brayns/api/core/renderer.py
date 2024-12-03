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
from enum import Enum
from typing import TypedDict, Unpack

from brayns.network.connection import Connection
from brayns.utils.composing import serialize
from brayns.utils.parsing import deserialize

from .material import Material
from .objects import (
    CreateObjectParams,
    Object,
    create_specific_object,
    get_specific_object,
    update_specific_object,
)
from .texture import Texture2D


class Renderer(Object): ...


class PixelFilter(Enum):
    POINT = "Point"
    BOX = "Box"
    GAUSS = "Gauss"
    MITCHELL_NETRAVALI = "MitchellNetravali"
    BLACKMAN_HARRIS = "BlackmanHarris"


Background = float | tuple[float, float, float] | tuple[float, float, float, float] | Texture2D


@dataclass
class GetRendererResult:
    materials: list[Material]
    samples: int
    max_recursion: int
    min_contribution: float
    variance_threshold: float
    background: Background
    max_depth: Texture2D | None
    pixel_filter: PixelFilter


class UpdateRendererParams(TypedDict, total=False):
    materials: list[Material]
    samples: int
    max_recursion: int
    min_contribution: float
    variance_threshold: float
    background: Background
    max_depth: Texture2D | None
    pixel_filter: PixelFilter


class CreateRendererParams(CreateObjectParams, UpdateRendererParams): ...


async def get_renderer(connection: Connection, renderer: Renderer) -> GetRendererResult:
    result = await get_specific_object(connection, "Renderer", renderer)
    return deserialize(result, GetRendererResult)


async def update_renderer(
    connection: Connection, renderer: Renderer, **settings: Unpack[UpdateRendererParams]
) -> None:
    await update_specific_object(connection, "Renderer", renderer, serialize(settings))


class AoRenderer(Renderer): ...


@dataclass
class GetAoRendererResult:
    ao_samples: int
    ao_distance: float
    ao_intensity: float
    volume_sampling_rate: float


class UpdateAoRendererParams(TypedDict, total=False):
    ao_samples: int
    ao_distance: float
    ao_intensity: float
    volume_sampling_rate: float


class CreateAoRendererParams(CreateRendererParams, UpdateAoRendererParams): ...


async def create_ao_renderer(connection: Connection, **settings: Unpack[CreateAoRendererParams]) -> AoRenderer:
    object = await create_specific_object(connection, "AoRenderer", serialize(settings))
    return AoRenderer(object.id)


async def get_ao_renderer(connection: Connection, renderer: AoRenderer) -> GetAoRendererResult:
    result = await get_specific_object(connection, "AoRenderer", renderer)
    return deserialize(result, GetAoRendererResult)


async def update_ao_renderer(
    connection: Connection, renderer: AoRenderer, **settings: Unpack[UpdateAoRendererParams]
) -> None:
    await update_specific_object(connection, "AoRenderer", renderer, serialize(settings))


class ScivisRenderer(Renderer): ...


@dataclass
class GetScivisRendererResult:
    shadows: bool
    ao_samples: int
    ao_distance: float
    volume_sampling_rate: float
    show_visible_lights: bool


class UpdateScivisRendererParams(TypedDict, total=False):
    shadows: bool
    ao_samples: int
    ao_distance: float
    volume_sampling_rate: float
    show_visible_lights: bool


class CreateScivisRendererParams(CreateRendererParams, UpdateScivisRendererParams): ...


async def create_scivis_renderer(
    connection: Connection, **settings: Unpack[CreateScivisRendererParams]
) -> ScivisRenderer:
    object = await create_specific_object(connection, "ScivisRenderer", serialize(settings))
    return ScivisRenderer(object.id)


async def get_scivis_renderer(connection: Connection, renderer: ScivisRenderer) -> GetScivisRendererResult:
    result = await get_specific_object(connection, "ScivisRenderer", renderer)
    return deserialize(result, GetScivisRendererResult)


async def update_scivis_renderer(
    connection: Connection, renderer: ScivisRenderer, **settings: Unpack[UpdateScivisRendererParams]
) -> None:
    await update_specific_object(connection, "ScivisRenderer", renderer, serialize(settings))


class PathTracer(Renderer): ...


@dataclass
class GetPathTracerResult: ...


class UpdatePathTracerParams(TypedDict, total=False): ...


class CreatePathTracerParams(CreateRendererParams, UpdatePathTracerParams): ...


async def create_path_tracer(connection: Connection, **settings: Unpack[CreatePathTracerParams]) -> PathTracer:
    object = await create_specific_object(connection, "PathTracer", serialize(settings))
    return PathTracer(object.id)


async def get_path_tracer(connection: Connection, renderer: PathTracer) -> GetPathTracerResult:
    result = await get_specific_object(connection, "PathTracer", renderer)
    return deserialize(result, GetPathTracerResult)


async def update_path_tracer(
    connection: Connection, renderer: PathTracer, **settings: Unpack[UpdatePathTracerParams]
) -> None:
    await update_specific_object(connection, "PathTracer", renderer, serialize(settings))
