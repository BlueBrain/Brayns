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

import os
from collections.abc import AsyncIterator, Iterator
from subprocess import PIPE, STDOUT, Popen

import pytest
import pytest_asyncio

from brayns import Connection, connect

HOST = os.getenv("BRAYNS_HOST", "localhost")
PORT = int(os.getenv("BRAYNS_PORT", "5000"))
EXECUTABLE = os.getenv("BRAYNS_EXECUTABLE", "braynsService")


def start_service() -> Popen[str]:
    return Popen(
        args=[
            EXECUTABLE,
            "--host",
            HOST,
            "--port",
            str(PORT),
        ],
        stdin=PIPE,
        stdout=PIPE,
        stderr=STDOUT,
        text=True,
    )


async def connect_to_service() -> Connection:
    return await connect(HOST, PORT, max_attempts=100)


@pytest.fixture(scope="session")
def service() -> Iterator[Popen[str]]:
    with start_service() as process:
        yield process
        process.terminate()


@pytest_asyncio.fixture
async def connection(service) -> AsyncIterator[Connection]:
    async with await connect_to_service() as connection:
        yield connection
