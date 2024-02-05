# Copyright 2015-2024 Blue Brain Project/EPFL
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

from typing import Any

from brayns.client.client import Client

from mock_server import MockServer


class MockClientAndServer:

    def __init__(
        self,
        host: str,
        port: int,
        request_handler: MockServer.RequestHandler,
        secure: bool = False,
        certfile: str = None,
        keyfile: str = None,
        password: str = None,
    ) -> None:
        self._server = MockServer(
            host=host,
            port=port,
            request_handler=request_handler,
            secure=secure,
            certfile=certfile,
            keyfile=keyfile,
            password=password
        )
        self._server.start()
        self._client = Client(
            uri=f'{host}:{port}',
            secure=secure,
            cafile=certfile
        )

    @property
    def client(self) -> Client:
        return self._client

    def close(self) -> None:
        self._client.disconnect()
        self._server.stop()
