# Copyright (c) 2015-2021 EPFL/Blue Brain Project
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
        if not isinstance(data, str):
            return None
        request = json.loads(data)
        reply = self._get_reply(request)
        if reply is None:
            return None
        return json.dumps(reply)

    def _get_reply(self, request: dict) -> dict:
        request_id = request.get('id')
        if request_id is None:
            return None
        return {
            'jsonrpc': '2.0',
            'id': request_id,
            'result': self._get_result(request)
        }

    def _get_result(self, request: dict) -> Any:
        method = request['method']
        if method == 'registry':
            return list(self._requests)
        if method == 'schema':
            return self._requests[
                request['params']['endpoint']
            ].schema
        return self._requests[method].result
