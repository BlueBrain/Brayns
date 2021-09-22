# Copyright (c) 2021 EPFL/Blue Brain Project
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
import unittest

import mock_requests
from mock_client_and_server import MockClientAndServer
from mock_request import MockRequest
from mock_request_handler import MockRequestHandler


class TestEntrypoints(unittest.TestCase):

    def __init__(self, *args, **kwargs) -> None:
        super().__init__(*args, **kwargs)
        self._requests = mock_requests.load()
        request_handler = MockRequestHandler(self._requests)
        self._client_and_server = MockClientAndServer(
            host='localhost',
            port=5000,
            request_handler=request_handler
        )
        ssl = pathlib.Path(__file__).parent / 'ssl'
        self._secure_client_and_server = MockClientAndServer(
            host='locahost',
            port=5001,
            request_handler=request_handler,
            secure=True,
            certfile=(ssl / 'certificate.pem').absolute(),
            keyfile=(ssl / 'key.pem').absolute(),
            password='test'
        )

    def test_all(self) -> None:
        for request in self._requests:
            self._run(self._client_and_server, request)
            self._run(self._secure_client_and_server, request)

    def _run(
        self,
        client_and_server: MockClientAndServer,
        request: MockRequest
    ) -> None:
        self.assertTrue(
            client_and_server.check_client_has_method(request.method)
        )
        self.assertEqual(
            client_and_server.request(request.method, request.params),
            request.result
        )


if __name__ == '__main__':
    unittest.main()
