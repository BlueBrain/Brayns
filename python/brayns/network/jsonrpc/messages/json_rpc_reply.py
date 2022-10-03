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
class JsonRpcReply:
    """Reply received from a request sent to a brayns instance.

    :param id: ID of the corresponding request.
    :type id: int | str
    :param result: Reply result (usually objects), defaults to None.
    :type result: Any, optional
    :param binary: Reply binary data, defaults to empty bytes.
    :type binary: bytes, optional
    """

    id: int | str | None
    result: Any = field(default=None, repr=False)
    binary: bytes = field(default=b'', repr=False)

    @staticmethod
    def for_notifications() -> JsonRpcReply:
        return JsonRpcReply(id=None)
