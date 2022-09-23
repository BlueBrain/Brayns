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

from dataclasses import dataclass, field

from .json_rpc_request import JsonRpcRequest


@dataclass
class Request(JsonRpcRequest):
    """Request to send to a running instance of brayns service.

    :param id: Request ID to monitor the request.
        No replies will be received if set to None.
    :type id: int | str | None
    :param method: JSON-RPC method.
    :type method: str
    :param params: Request parameters (usually objects).
    :type params: Any, optional
    """

    binary: bytes = field(default=b'', repr=False)
