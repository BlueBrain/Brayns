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

from dataclasses import dataclass

from brayns.instance.jsonrpc.json_rpc_id import JsonRpcId
from brayns.instance.request_progress import RequestProgress


@dataclass
class JsonRpcProgress:

    id: JsonRpcId
    params: RequestProgress

    @staticmethod
    def from_dict(message: dict) -> 'JsonRpcProgress':
        params = message['params']
        return JsonRpcProgress(
            id=params['id'],
            params=RequestProgress(
                operation=params['operation'],
                amount=params['amount']
            )
        )
