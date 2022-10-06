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

from __future__ import annotations

from dataclasses import dataclass, field
from typing import Any


@dataclass
class JsonRpcRequest:
    """Request to send to a running instance of brayns service.

    :param id: Request ID to monitor the request.
        No replies will be received if set to None.
    :type id: int | str | None
    :param method: JSON-RPC method.
    :type method: str
    :param params: Request parameters (usually objects), defaults to None.
    :type params: Any, optional
    :param binary: Request binary data, defaults to empty bytes.
    :type binary: bytes, optional
    """

    id: int | str | None
    method: str
    params: Any = field(default=None, repr=False)
    binary: bytes = field(default=b'', repr=False)
