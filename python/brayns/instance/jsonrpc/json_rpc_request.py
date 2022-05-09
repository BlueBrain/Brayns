# Copyright (c) 2015-2022 EPFL/Blue Brain Project
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
from dataclasses import dataclass
from typing import Any, Union


@dataclass
class JsonRpcRequest:

    id: Union[int, str, None]
    method: str
    params: Any = None

    def is_notification(self) -> bool:
        return self.id is None

    def to_dict(self) -> dict:
        message = {
            'jsonrpc': '2.0',
            'method': self.method
        }
        if self.id is not None:
            message['id'] = self.id
        if self.params is not None:
            message['params'] = self.params
        return message

    def to_json(self) -> str:
        return json.dumps(self.to_dict())
