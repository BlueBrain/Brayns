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

import pytest

from brayns import (
    BezierBasis,
    BsplineBasis,
    CatmullRomBasis,
    Connection,
    CurveType,
    HermiteBasis,
    JsonRpcError,
    LinearBasis,
    VoxelType,
    create_boxes,
    create_curve,
    create_cylinders,
    create_discs,
    create_isosurfaces,
    create_planes,
    create_quad_mesh,
    create_regular_volume,
    create_ribbon,
    create_spheres,
    create_triangle_mesh,
    get_boxes,
    get_curve,
    get_cylinders,
    get_discs,
    get_isosurfaces,
    get_planes,
    get_quad_mesh,
    get_ribbon,
    get_spheres,
    get_triangle_mesh,
)


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_triangle_mesh(connection: Connection) -> None:
    minimal = await create_triangle_mesh(connection, positions=[(0, 0, 0), (1, 1, 1), (1, 0, 1)])

    settings = await get_triangle_mesh(connection, minimal)
    assert settings.positions
    assert not settings.normals
    assert not settings.colors
    assert not settings.uvs
    assert not settings.indices

    mesh = await create_triangle_mesh(
        connection,
        positions=[(0, 0, 0), (1, 1, 1), (1, 0, 1)],
        normals=[(0, 0, 0), (1, 1, 1), (1, 0, 1)],
        colors=[(1, 0, 0, 1), (0, 1, 0, 1), (0, 0, 1, 1)],
        uvs=[(1, 0), (0, 1), (1, 1)],
        indices=[(0, 1, 2)],
    )

    settings = await get_triangle_mesh(connection, mesh)
    assert settings.positions
    assert settings.normals
    assert settings.colors
    assert settings.uvs
    assert settings.indices

    with pytest.raises(JsonRpcError):
        await create_triangle_mesh(connection, positions=[])

    with pytest.raises(JsonRpcError):
        await create_triangle_mesh(connection, positions=[(0, 1, 2), (4, 5, 6)], normals=[(0, 1, 2)])


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_quad_mesh(connection: Connection) -> None:
    mesh = await create_quad_mesh(
        connection,
        positions=[(0, 0, 0), (1, 1, 1), (1, 0, 1), (0, 0, 1)],
        normals=[(0, 0, 0), (1, 1, 1), (1, 0, 1), (0, 0, 1)],
        colors=[(1, 0, 0, 1), (0, 1, 0, 1), (0, 0, 1, 1), (0, 0, 1, 1)],
        uvs=[(1, 0), (0, 1), (1, 1), (1, 0)],
        indices=[(0, 1, 2, 3)],
    )

    settings = await get_quad_mesh(connection, mesh)
    assert settings.positions
    assert settings.normals
    assert settings.colors
    assert settings.uvs
    assert settings.indices


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_spheres(connection: Connection) -> None:
    spheres = await create_spheres(
        connection,
        positions_radii=[(0, 0, 0, 1)],
        uvs=[(0, 0)],
    )

    settings = await get_spheres(connection, spheres)
    assert settings.positions_radii
    assert settings.uvs


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_discs(connection: Connection) -> None:
    discs = await create_discs(
        connection,
        positions_radii=[(0, 0, 0, 1)],
        uvs=[(0, 0)],
        normals=[(1, 1, 1)],
    )

    settings = await get_discs(connection, discs)
    assert settings.positions_radii
    assert settings.uvs
    assert settings.normals


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_cylinders(connection: Connection) -> None:
    cylinders = await create_cylinders(
        connection,
        positions_radii=[(0, 0, 0, 1), (1, 1, 1, 2)],
        indices=[0],
        colors=[(1, 1, 1, 1), (0, 0, 0, 1)],
        uvs=[(0, 0), (1, 1)],
    )

    settings = await get_cylinders(connection, cylinders)
    assert settings.positions_radii
    assert settings.indices
    assert settings.colors
    assert settings.uvs


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_curve(connection: Connection) -> None:
    bases = [
        LinearBasis(),
        BezierBasis(),
        BsplineBasis(),
        HermiteBasis([(0, 0, 1, 1), (1, 0, 0, 1), (0, 1, 0, 1), (1, 1, 0, 1)]),
        CatmullRomBasis(),
    ]

    types = list(CurveType)

    for t in types:
        for basis in bases:
            curve = await create_curve(
                connection,
                positions_radii=[(0, 0, 0, 1), (1, 1, 1, 2), (1, 0, 1, 2), (0, 1, 1, 2)],
                indices=[0],
                colors=[(1, 1, 1, 1), (0, 0, 0, 1), (0, 1, 1, 1), (1, 1, 0, 1)],
                uvs=[(0, 0), (1, 1), (0, 1), (1, 0)],
                type=t,
                basis=basis,
            )

            settings = await get_curve(connection, curve)
            assert settings.positions_radii
            assert settings.indices
            assert settings.colors
            assert settings.uvs
            assert settings.type == t
            assert settings.basis == basis


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_ribbon(connection: Connection) -> None:
    bases = [
        BezierBasis(),
        BsplineBasis(),
        HermiteBasis([(0, 0, 1, 1), (1, 0, 0, 1), (0, 1, 0, 1), (1, 1, 0, 1)]),
        CatmullRomBasis(),
    ]

    for basis in bases:
        ribbon = await create_ribbon(
            connection,
            positions_radii=[(0, 0, 0, 1), (1, 1, 1, 2), (1, 0, 1, 2), (0, 1, 1, 2)],
            indices=[0],
            colors=[(1, 1, 1, 1), (0, 0, 0, 1), (0, 1, 1, 1), (1, 1, 0, 1)],
            uvs=[(0, 0), (1, 1), (0, 1), (1, 0)],
            normals=[(1, 1, 1), (0, 0, 1), (1, 0, 0), (0, 1, 0)],
            basis=basis,
        )

        settings = await get_ribbon(connection, ribbon)
        assert settings.positions_radii
        assert settings.indices
        assert settings.colors
        assert settings.uvs
        assert settings.normals
        assert settings.basis == basis


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_boxes(connection: Connection) -> None:
    boxes = await create_boxes(connection, boxes=[((0, 0, 0), (1, 1, 1))])

    settings = await get_boxes(connection, boxes)
    assert settings.boxes


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_planes(connection: Connection) -> None:
    planes = await create_planes(connection, coefficients=[(1, 1, 1, 0)])

    settings = await get_planes(connection, planes)
    assert settings.coefficients
    assert not settings.bounds

    planes = await create_planes(
        connection,
        coefficients=[(1, 1, 1, 0)],
        bounds=[((0, 0, 0), (1, 1, 1))],
    )

    settings = await get_planes(connection, planes)
    assert settings.coefficients
    assert settings.bounds


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_isosurfaces(connection: Connection) -> None:
    volume = await create_regular_volume(connection, data=8 * b"\1", voxel_type=VoxelType.U8, size=(2, 2, 2))

    isosurfaces = await create_isosurfaces(connection, volume=volume, isovalues=[1])

    settings = await get_isosurfaces(connection, isosurfaces)
    assert settings.volume == volume
    assert settings.isovalues
