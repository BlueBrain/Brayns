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

from logging import DEBUG
import os
from collections.abc import AsyncIterator

import pytest_asyncio

from brayns import Connection, connect, create_logger, clear_objects

HOST = os.getenv("BRAYNS_HOST", "localhost")
PORT = int(os.getenv("BRAYNS_PORT", "5000"))


async def connect_to_service() -> Connection:
    logger = create_logger(DEBUG)
    return await connect(HOST, PORT, max_attempts=10, logger=logger)


@pytest_asyncio.fixture
async def connection() -> AsyncIterator[Connection]:
    async with await connect_to_service() as connection:
        await clear_objects(connection)
        yield connection
