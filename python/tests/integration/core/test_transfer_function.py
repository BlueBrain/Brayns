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
    Box1,
    Color4,
    Connection,
    LinearTransferFunctionSettings,
    create_linear_transfer_function,
    get_linear_transfer_function,
    get_linear_transfer_function_settings,
    update_linear_transfer_function_settings,
)


@pytest.mark.integration_test
@pytest.mark.asyncio
async def test_linear_transfer_function(connection: Connection) -> None:
    settings = LinearTransferFunctionSettings()
    linear_transfer_function = await create_linear_transfer_function(connection, settings)

    assert linear_transfer_function.id == 1
    assert linear_transfer_function.settings == settings

    retreived = await get_linear_transfer_function(connection, linear_transfer_function.id)
    assert retreived.id == linear_transfer_function.id
    assert retreived.settings == linear_transfer_function.settings

    linear_transfer_function.settings.scalar_range = Box1(-1, 2)
    await linear_transfer_function.push(connection)

    settings = await get_linear_transfer_function_settings(connection, linear_transfer_function.id)
    assert settings == linear_transfer_function.settings

    settings.colors = [Color4(0, 1, 0)]
    await update_linear_transfer_function_settings(connection, linear_transfer_function.id, settings)

    await linear_transfer_function.pull(connection)
    assert linear_transfer_function.settings == settings
