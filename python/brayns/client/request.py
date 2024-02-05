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
from typing import Any, Union


class Request:

    def __init__(
        self,
        method: str,
        params: Any = None,
        request_id: Union[None, int, str] = None,
        jsonrpc: str = '2.0'
    ) -> None:
        self.method = method
        self.params = params
        self.request_id = request_id
        self.jsonrpc = jsonrpc

    def is_notification(self) -> bool:
        return self.request_id is None

    def to_json(self) -> str:
        message = {
            'jsonrpc': self.jsonrpc,
            'method': self.method
        }
        if self.params is not None:
            message['params'] = self.params
        if self.request_id is not None:
            message['id'] = self.request_id
        return json.dumps(message)
