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
    ToneMapperSettings,
    create_tone_mapper,
    get_tone_mapper,
    get_tone_mapper_settings,
    update_tone_mapper_settings,
)


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_tone_mapper(connection: Connection) -> None:
    settings = ToneMapperSettings()
    tone_mapper = await create_tone_mapper(connection, settings)

    assert tone_mapper.id == 1
    assert tone_mapper.settings == settings

    retreived = await get_tone_mapper(connection, tone_mapper.id)
    assert retreived.id == tone_mapper.id
    assert retreived.settings == tone_mapper.settings

    tone_mapper.settings.aces_color = False
    await tone_mapper.push(connection)

    settings = await get_tone_mapper_settings(connection, tone_mapper.id)
    assert settings == tone_mapper.settings

    settings.contrast = 2
    await update_tone_mapper_settings(connection, tone_mapper.id, settings)

    await tone_mapper.pull(connection)
    assert tone_mapper.settings == settings
