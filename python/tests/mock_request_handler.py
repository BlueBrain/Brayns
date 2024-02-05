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

import json
from typing import Any, List, Union

from mock_request import MockRequest


class MockRequestHandler:

    def __init__(self, requests: List[MockRequest]) -> None:
        self._requests = {
            request.method: request
            for request in requests
        }

    def __call__(self, data: Union[bytes, str]) -> Union[bytes, str, None]:
        request = json.loads(data)
        try:
            reply = self._get_reply(request, self._get_result(request))
        except Exception as e:
            reply = self._get_error(request, str(e))
        return None if reply is None else json.dumps(reply)

    def _get_result(self, request: dict) -> Any:
        method = request['method']
        if method == 'registry':
            return sorted(self._requests)
        if method == 'schema':
            endpoint = request['params']['endpoint']
            return self._requests[endpoint].schema
        return self._reply_mock(method, request)

    def _reply_mock(self, method: str, request: dict) -> dict:
        mock = self._requests[method]
        params = request.get('params') or {}
        mock_params = mock.params or {}
        if params != mock_params:
            raise ValueError(f'Expected param {mock_params} got {params}')
        return mock.result

    def _get_message(self, request: dict, key: str, value: Any):
        request_id = request.get('id')
        if request_id is None:
            return None
        return {
            'jsonrpc': '2.0',
            'id': request_id,
            key: value
        }

    def _get_reply(self, request: dict, result: Any):
        return self._get_message(
            request,
            key='result',
            value=result
        )

    def _get_error(self, request: dict, message: str):
        return self._get_message(
            request,
            key='error',
            value={'message': message}
        )
