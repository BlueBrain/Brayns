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
from typing import Required, TypedDict, Unpack

from brayns.network.connection import Connection
from brayns.utils.composing import serialize
from brayns.utils.parsing import deserialize

from .objects import CreateObjectParams, Object, create_specific_object, get_specific_object


class Geometry(Object): ...


@dataclass
class GetMeshResult:
    positions: list[tuple[float, float, float]]
    normals: list[tuple[float, float, float]]
    colors: list[tuple[float, float, float, float]]
    uvs: list[tuple[float, float]]


class CreateMeshParams(TypedDict, total=False):
    positions: Required[list[tuple[float, float, float]]]
    normals: list[tuple[float, float, float]]
    colors: list[tuple[float, float, float, float]]
    uvs: list[tuple[float, float]]


class TriangleMesh(Geometry): ...


@dataclass
class GetTriangleMeshResult(GetMeshResult):
    indices: list[tuple[int, int, int]]


class CreateTriangleMeshParams(CreateObjectParams, CreateMeshParams, total=False):
    indices: list[tuple[int, int, int]]


async def create_triangle_mesh(connection: Connection, **settings: Unpack[CreateTriangleMeshParams]) -> TriangleMesh:
    object = await create_specific_object(connection, "TriangleMesh", serialize(settings))
    return TriangleMesh(object.id)


async def get_triangle_mesh(connection: Connection, mesh: TriangleMesh) -> GetTriangleMeshResult:
    result = await get_specific_object(connection, "TriangleMesh", mesh)
    return deserialize(result, GetTriangleMeshResult)


class QuadMesh(Geometry): ...


@dataclass
class GetQuadMeshResult(GetMeshResult):
    indices: list[tuple[int, int, int, int]]


class CreateQuadMeshParams(CreateObjectParams, CreateMeshParams, total=False):
    indices: list[tuple[int, int, int, int]]


async def create_quad_mesh(connection: Connection, **settings: Unpack[CreateQuadMeshParams]) -> QuadMesh:
    object = await create_specific_object(connection, "QuadMesh", serialize(settings))
    return QuadMesh(object.id)


async def get_quad_mesh(connection: Connection, mesh: QuadMesh) -> GetQuadMeshResult:
    result = await get_specific_object(connection, "QuadMesh", mesh)
    return deserialize(result, GetQuadMeshResult)
