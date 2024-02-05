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

import pathlib

import mock_requests
from mock_client_and_server import MockClientAndServer
from mock_request_handler import MockRequestHandler


class BraynsTester:

    def __init__(
        self,
        requests_folder: pathlib.Path,
        ssl_folder: pathlib.Path,
        host: str = 'localhost',
        port: int = 5000,
        secure_port: int = 5001,
        ssl_certfile: str = 'certificate.pem',
        ssl_keyfile: str = 'key.pem',
        ssl_password: str = 'test'
    ) -> None:
        self._requests = mock_requests.load(requests_folder)
        request_handler = MockRequestHandler(self._requests)
        self._client_and_server = MockClientAndServer(
            host=host,
            port=port,
            request_handler=request_handler
        )
        self._secure_client_and_server = MockClientAndServer(
            host=host,
            port=secure_port,
            request_handler=request_handler,
            secure=True,
            certfile=str(ssl_folder / ssl_certfile),
            keyfile=str(ssl_folder / ssl_keyfile),
            password=ssl_password
        )

    @property
    def requests(self):
        return self._requests

    @property
    def client(self):
        return self._client_and_server.client

    @property
    def secure_client(self):
        return self._secure_client_and_server.client

    def close_connections(self):
        self._client_and_server.close()
        self._secure_client_and_server.close()
