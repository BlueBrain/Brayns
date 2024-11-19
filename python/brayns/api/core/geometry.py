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
from typing import Literal, Required, TypedDict, Unpack

from brayns.network.connection import Connection
from brayns.utils.composing import serialize
from brayns.utils.parsing import deserialize

from .objects import CreateObjectParams, Object, create_specific_object, get_specific_object
from .volume import Volume


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


class Spheres(Geometry): ...


@dataclass
class GetSpheresResult:
    positions_radii: list[tuple[float, float, float, float]]
    uvs: list[tuple[float, float]]


class CreateSpheresParams(CreateObjectParams, total=False):
    positions_radii: Required[list[tuple[float, float, float, float]]]
    uvs: list[tuple[float, float]]


async def create_spheres(connection: Connection, **settings: Unpack[CreateSpheresParams]) -> Spheres:
    object = await create_specific_object(connection, "Spheres", serialize(settings))
    return Spheres(object.id)


async def get_spheres(connection: Connection, spheres: Spheres) -> GetSpheresResult:
    result = await get_specific_object(connection, "Spheres", spheres)
    return deserialize(result, GetSpheresResult)


class Discs(Geometry): ...


@dataclass
class GetDiscsResult(GetSpheresResult):
    normals: list[tuple[float, float, float]]


class CreateDiscsParams(CreateSpheresParams, total=False):
    normals: list[tuple[float, float, float]]


async def create_discs(connection: Connection, **settings: Unpack[CreateDiscsParams]) -> Discs:
    object = await create_specific_object(connection, "Discs", serialize(settings))
    return Discs(object.id)


async def get_discs(connection: Connection, discs: Discs) -> GetDiscsResult:
    result = await get_specific_object(connection, "Discs", discs)
    return deserialize(result, GetDiscsResult)


class Cylinders(Geometry): ...


@dataclass
class GetCylindersResult:
    positions_radii: list[tuple[float, float, float, float]]
    indices: list[int]
    colors: list[tuple[float, float, float, float]]
    uvs: list[tuple[float, float]]


class CreateCylindersParams(CreateObjectParams, total=False):
    positions_radii: Required[list[tuple[float, float, float, float]]]
    indices: Required[list[int]]
    colors: list[tuple[float, float, float, float]]
    uvs: list[tuple[float, float]]


async def create_cylinders(connection: Connection, **settings: Unpack[CreateCylindersParams]) -> Cylinders:
    object = await create_specific_object(connection, "Cylinders", serialize(settings))
    return Cylinders(object.id)


async def get_cylinders(connection: Connection, cylinders: Cylinders) -> GetCylindersResult:
    result = await get_specific_object(connection, "Cylinders", cylinders)
    return deserialize(result, GetCylindersResult)


class Curve(Geometry): ...


class CurveType(Enum):
    FLAT = "Flat"
    ROUND = "Round"


@dataclass(frozen=True)
class LinearBasis:
    type: Literal["Linear"] = "Linear"


@dataclass(frozen=True)
class BezierBasis:
    type: Literal["Bezier"] = "Bezier"


@dataclass(frozen=True)
class BsplineBasis:
    type: Literal["Bspline"] = "Bspline"


@dataclass(frozen=True)
class HermiteBasis:
    tangents: list[tuple[float, float, float, float]]
    type: Literal["Hermite"] = "Hermite"


@dataclass(frozen=True)
class CatmullRomBasis:
    type: Literal["CatmullRom"] = "CatmullRom"


CurveBasis = LinearBasis | BezierBasis | BsplineBasis | HermiteBasis | CatmullRomBasis


@dataclass
class GetCurveResult(GetCylindersResult):
    type: CurveType
    basis: CurveBasis


class CreateCurveParams(CreateCylindersParams, total=False):
    type: CurveType
    basis: CurveBasis


async def create_curve(connection: Connection, **settings: Unpack[CreateCurveParams]) -> Curve:
    object = await create_specific_object(connection, "Curve", serialize(settings))
    return Curve(object.id)


async def get_curve(connection: Connection, curve: Curve) -> GetCurveResult:
    result = await get_specific_object(connection, "Curve", curve)
    return deserialize(result, GetCurveResult)


class Ribbon(Geometry): ...


RibbonBasis = BezierBasis | BsplineBasis | HermiteBasis | CatmullRomBasis


@dataclass
class GetRibbonResult(GetCylindersResult):
    normals: list[tuple[float, float, float]]
    basis: RibbonBasis


class CreateRibbonParams(CreateCylindersParams, total=False):
    normals: Required[list[tuple[float, float, float]]]
    basis: RibbonBasis


async def create_ribbon(connection: Connection, **settings: Unpack[CreateRibbonParams]) -> Ribbon:
    object = await create_specific_object(connection, "Ribbon", serialize(settings))
    return Ribbon(object.id)


async def get_ribbon(connection: Connection, ribbon: Ribbon) -> GetRibbonResult:
    result = await get_specific_object(connection, "Ribbon", ribbon)
    return deserialize(result, GetRibbonResult)


class Boxes(Geometry): ...


@dataclass
class GetBoxesResult:
    boxes: list[tuple[tuple[float, float, float], tuple[float, float, float]]]


class CreateBoxesParams(CreateObjectParams, total=False):
    boxes: Required[list[tuple[tuple[float, float, float], tuple[float, float, float]]]]


async def create_boxes(connection: Connection, **settings: Unpack[CreateBoxesParams]) -> Boxes:
    object = await create_specific_object(connection, "Boxes", serialize(settings))
    return Boxes(object.id)


async def get_boxes(connection: Connection, boxes: Boxes) -> GetBoxesResult:
    result = await get_specific_object(connection, "Boxes", boxes)
    return deserialize(result, GetBoxesResult)


class Planes(Geometry): ...


@dataclass
class GetPlanesResult:
    coefficients: list[tuple[float, float, float, float]]
    bounds: list[tuple[tuple[float, float, float], tuple[float, float, float]]]


class CreatePlanesParams(CreateObjectParams, total=False):
    coefficients: Required[list[tuple[float, float, float, float]]]
    bounds: list[tuple[tuple[float, float, float], tuple[float, float, float]]]


async def create_planes(connection: Connection, **settings: Unpack[CreatePlanesParams]) -> Planes:
    object = await create_specific_object(connection, "Planes", serialize(settings))
    return Planes(object.id)


async def get_planes(connection: Connection, planes: Planes) -> GetPlanesResult:
    result = await get_specific_object(connection, "Planes", planes)
    return deserialize(result, GetPlanesResult)


class Isosurfaces(Geometry): ...


@dataclass
class GetIsosurfacesResult:
    volume: Volume
    isovalues: list[float]


class CreateIsosurfacesParams(CreateObjectParams, total=False):
    volume: Required[Volume]
    isovalues: Required[list[float]]


async def create_isosurfaces(connection: Connection, **settings: Unpack[CreateIsosurfacesParams]) -> Isosurfaces:
    object = await create_specific_object(connection, "Isosurfaces", serialize(settings))
    return Isosurfaces(object.id)


async def get_isosurfaces(connection: Connection, isosurfaces: Isosurfaces) -> GetIsosurfacesResult:
    result = await get_specific_object(connection, "Isosurfaces", isosurfaces)
    return deserialize(result, GetIsosurfacesResult)
