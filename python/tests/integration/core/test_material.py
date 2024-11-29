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
    MaterialTexture2D,
    MaterialVolumeTexture,
    TextureFormat,
    VoxelType,
    create_ao_material,
    create_linear_transfer_function,
    create_principled_material,
    create_regular_volume,
    create_scivis_material,
    create_texture2d,
    create_volume_texture,
    get_ao_material,
    get_principled_material,
    get_scivis_material,
    remove_objects,
    update_ao_material,
    update_principled_material,
    update_scivis_material,
)


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_ao_material(connection: Connection) -> None:
    material = await create_ao_material(connection, diffuse=(1, 1, 1), opacity=1)

    settings = await get_ao_material(connection, material)
    assert settings.diffuse == (1, 1, 1)
    assert settings.opacity == 1

    await update_ao_material(connection, material, opacity=0.5)

    settings = await get_ao_material(connection, material)
    assert settings.diffuse == (1, 1, 1)
    assert settings.opacity == 0.5


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_scivis_material(connection: Connection) -> None:
    material = await create_scivis_material(
        connection,
        diffuse=(1, 1, 1),
        opacity=1,
        specular=(0.1, 0.1, 0.1),
        shininess=2,
        transparency_filter=(0, 0, 0),
    )

    settings = await get_scivis_material(connection, material)
    assert settings.diffuse == (1, 1, 1)
    assert settings.opacity == 1
    assert settings.specular == (0.1, 0.1, 0.1)
    assert settings.shininess == 2
    assert settings.transparency_filter == (0, 0, 0)

    await update_scivis_material(connection, material, opacity=0.5, specular=(0, 0, 1))

    settings = await get_scivis_material(connection, material)
    assert settings.diffuse == (1, 1, 1)
    assert settings.opacity == 0.5
    assert settings.specular == (0, 0, 1)
    assert settings.shininess == 2
    assert settings.transparency_filter == (0, 0, 0)


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_principled_material(connection: Connection) -> None:
    material = await create_principled_material(
        connection,
        base_color=(1, 1, 1),
        edge_color=(1, 1, 1),
        metallic=1.0,
        diffuse=1.0,
        specular=1.0,
        ior=1.0,
        transmission=1.0,
        transmission_color=(1, 1, 1),
        transmission_depth=1.0,
        roughness=1.0,
        anisotropy=1.0,
        rotation=1.0,
        normal=1.0,
        base_normal=1.0,
        thin=False,
        thickness=1.0,
        backlight=1.0,
        coat=1.0,
        coat_ior=1.0,
        coat_color=(1, 1, 1),
        coat_thickness=1.0,
        coat_roughness=1.0,
        coat_normal=1.0,
        sheen=1.0,
        sheen_color=(1, 1, 1),
        sheen_tint=1.0,
        sheen_roughness=1.0,
        opacity=1.0,
        emissive_color=(1, 1, 1),
    )

    settings = await get_principled_material(connection, material)
    assert settings.base_color == (1, 1, 1)
    assert settings.opacity == 1
    assert settings.coat == 1

    await update_principled_material(connection, material, opacity=0.5)

    settings = await get_principled_material(connection, material)
    assert settings.base_color == (1, 1, 1)
    assert settings.opacity == 0.5
    assert settings.coat == 1


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_texture2d(connection: Connection) -> None:
    data = 4 * b"\1"

    texture = await create_texture2d(
        connection,
        data,
        format=TextureFormat.L8,
        size=(2, 2),
    )

    material = await create_scivis_material(
        connection,
        diffuse=MaterialTexture2D(texture),
        opacity=MaterialTexture2D(texture, factor=2),
    )

    settings = await get_scivis_material(connection, material)
    assert settings.diffuse == MaterialTexture2D(texture)
    assert settings.opacity == MaterialTexture2D(texture, factor=2)

    await remove_objects(connection, [texture])

    settings = await get_scivis_material(connection, material)

    assert isinstance(settings.diffuse, MaterialTexture2D)
    assert settings.diffuse.texture2d.id == 0

    assert isinstance(settings.opacity, MaterialTexture2D)
    assert settings.opacity.texture2d.id == 0


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_volume_texture(connection: Connection) -> None:
    data = 8 * b"\1"

    volume = await create_regular_volume(
        connection,
        data,
        voxel_type=VoxelType.U8,
        size=(2, 2, 2),
    )

    transfer_function = await create_linear_transfer_function(connection)

    texture = await create_volume_texture(
        connection,
        volume=volume,
        transfer_function=transfer_function,
    )

    material = await create_scivis_material(
        connection,
        diffuse=MaterialVolumeTexture(texture),
        opacity=MaterialVolumeTexture(texture, factor=2),
    )

    settings = await get_scivis_material(connection, material)
    assert settings.diffuse == MaterialVolumeTexture(texture)
    assert settings.opacity == MaterialVolumeTexture(texture, factor=2)

    await remove_objects(connection, [texture, volume])

    settings = await get_scivis_material(connection, material)

    assert isinstance(settings.diffuse, MaterialVolumeTexture)
    assert settings.diffuse.volume_texture.id == 0

    assert isinstance(settings.opacity, MaterialVolumeTexture)
    assert settings.opacity.volume_texture.id == 0
