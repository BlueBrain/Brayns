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
import unittest
from typing import Callable

from brayns.client.client import Client

from brayns_tester import BraynsTester
from mock_request import MockRequest


class TestEntrypoints(unittest.TestCase):

    REQUESTS = pathlib.Path(__file__).parent / 'requests'

    def setUp(self) -> None:
        self._tester = BraynsTester(
            requests_folder=pathlib.Path(__file__).parent / 'requests',
            ssl_folder=pathlib.Path(__file__).parent / 'ssl'
        )

    def tearDown(self) -> None:
        self._tester.close_connections()

    def test_requests(self) -> None:
        for request in self._tester.requests:
            self._check_request(self._tester.client, request)
            self._check_request(self._tester.secure_client, request)

    def test_methods(self) -> None:
        for request in self._tester.requests:
            self._check_method(self._tester.client, request)
            self._check_method(self._tester.secure_client, request)

    def _check_request(
        self,
        client: Client,
        request: MockRequest
    ) -> None:
        self.assertEqual(
            client.request(request.method, request.params),
            request.result
        )

    def _check_method(
        self,
        client: Client,
        request: MockRequest
    ) -> None:
        method = getattr(
            client,
            request.method.replace('-', '_'),
            None
        )
        self.assertIsNotNone(method)
        result = self._call_method(method, request)
        self.assertEqual(result, request.result)

    def _call_method(self, method: Callable, request: MockRequest):
        schemas = request.schema['params']
        if not schemas:
            return method()
        params = request.params
        schema = schemas[0]
        if isinstance(schema, dict) and 'oneOf' in schema:
            return method(params)
        return method(**params)


if __name__ == '__main__':
    unittest.main()
