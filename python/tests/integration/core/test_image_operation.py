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
    create_tone_mapper,
    get_tone_mapper,
    update_tone_mapper,
)


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_tone_mapper(connection: Connection) -> None:
    tone_mapper = await create_tone_mapper(connection)

    settings = await get_tone_mapper(connection, tone_mapper)

    settings.aces_color = False
    await update_tone_mapper(connection, tone_mapper, aces_color=False)

    assert settings == await get_tone_mapper(connection, tone_mapper)
