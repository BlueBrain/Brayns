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
    Connection,
    VoxelType,
    create_ambient_light,
    create_boxes,
    create_geometric_model,
    create_group,
    create_instance,
    create_linear_transfer_function,
    create_regular_volume,
    create_spheres,
    create_volumetric_model,
    create_world,
    get_geometric_model,
    get_group,
    get_instance,
    get_volumetric_model,
    get_world,
    remove_objects,
)


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_geometric_model(connection: Connection) -> None:
    spheres = await create_spheres(connection, positions_radii=[(0, 0, 0, 1)])

    model = await create_geometric_model(
        connection,
        geometry=spheres,
        materials=[0],
        colors=[(1, 0, 0, 1)],
        indices=[0],
        invert_normals=True,
        id=1,
    )

    settings = await get_geometric_model(connection, model)
    assert settings.geometry == spheres
    assert settings.materials == [0]
    assert settings.colors == [(1, 0, 0, 1)]
    assert settings.indices == [0]
    assert settings.invert_normals
    assert settings.id == 1

    await remove_objects(connection, [spheres])

    settings = await get_geometric_model(connection, model)
    assert settings.geometry.id == 0


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_volumetric_model(connection: Connection) -> None:
    volume = await create_regular_volume(connection, data=8 * b"\1", voxel_type=VoxelType.U8, size=(2, 2, 2))

    transfer_function = await create_linear_transfer_function(connection)

    model = await create_volumetric_model(
        connection,
        volume=volume,
        transfer_function=transfer_function,
        density_scale=2,
        anisotropy=0.5,
        id=1,
    )

    settings = await get_volumetric_model(connection, model)
    assert settings.volume == volume
    assert settings.transfer_function == transfer_function
    assert settings.density_scale == 2
    assert settings.anisotropy == 0.5
    assert settings.id == 1

    await remove_objects(connection, [volume, transfer_function])

    settings = await get_volumetric_model(connection, model)
    assert settings.volume.id == 0
    assert settings.transfer_function.id == 0


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_group(connection: Connection) -> None:
    spheres = await create_spheres(connection, positions_radii=[(0, 0, 0, 1)])
    geometric_model = await create_geometric_model(connection, geometry=spheres)

    volume = await create_regular_volume(connection, data=8 * b"\1", voxel_type=VoxelType.U8, size=(2, 2, 2))
    transfer_function = await create_linear_transfer_function(connection)
    volumetric_model = await create_volumetric_model(connection, volume=volume, transfer_function=transfer_function)

    light = await create_ambient_light(connection)

    group = await create_group(
        connection,
        geometries=[geometric_model],
        clipping_geometries=[geometric_model],
        volumes=[volumetric_model],
        lights=[light],
    )

    settings = await get_group(connection, group)
    assert settings.geometries == [geometric_model]
    assert settings.clipping_geometries == [geometric_model]
    assert settings.volumes == [volumetric_model]
    assert settings.lights == [light]
    assert settings.bounds == ((-1, -1, -1), (1, 1, 1))

    await remove_objects(connection, [spheres, geometric_model, volume, volumetric_model, transfer_function, light])

    settings = await get_group(connection, group)
    assert all(item.id == 0 for item in settings.geometries)
    assert all(item.id == 0 for item in settings.clipping_geometries)
    assert all(item.id == 0 for item in settings.volumes)
    assert all(item.id == 0 for item in settings.lights)
    assert settings.bounds == ((-1, -1, -1), (1, 1, 1))


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_instance(connection: Connection) -> None:
    boxes = await create_boxes(connection, boxes=[((0, 0, 0), (1, 1, 1))])
    geometric_model = await create_geometric_model(connection, geometry=boxes)
    group = await create_group(connection, geometries=[geometric_model])

    instance = await create_instance(
        connection,
        group=group,
        translation=(1, 1, 1),
        rotation=(0, 0, 0, 1),
        scale=(2, 2, 1),
        id=12,
    )

    settings = await get_instance(connection, instance)
    assert settings.group == group
    assert settings.translation == (1, 1, 1)
    assert settings.rotation == (0, 0, 0, 1)
    assert settings.scale == (2, 2, 1)
    assert settings.id == 12
    assert settings.bounds == ((1, 1, 1), (3, 3, 2))

    await remove_objects(connection, [boxes, geometric_model, group])

    settings = await get_instance(connection, instance)
    assert settings.group.id == 0
    assert settings.translation == (1, 1, 1)
    assert settings.rotation == (0, 0, 0, 1)
    assert settings.scale == (2, 2, 1)
    assert settings.id == 12
    assert settings.bounds == ((1, 1, 1), (3, 3, 2))


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_world(connection: Connection) -> None:
    boxes = await create_boxes(connection, boxes=[((0, 0, 0), (1, 1, 1))])
    geometric_model = await create_geometric_model(connection, geometry=boxes)
    group = await create_group(connection, geometries=[geometric_model])

    instances = [await create_instance(connection, group=group) for _ in range(3)]

    world = await create_world(connection, instances=instances)

    settings = await get_world(connection, world)
    assert settings.instances == instances
    assert settings.bounds == ((0, 0, 0), (1, 1, 1))

    await remove_objects(connection, [boxes, geometric_model, group, *instances])

    settings = await get_world(connection, world)
    assert all(instance.id == 0 for instance in settings.instances)
    assert settings.bounds == ((0, 0, 0), (1, 1, 1))
