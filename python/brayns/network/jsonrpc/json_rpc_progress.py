# Copyright (c) 2015-2023 EPFL/Blue Brain Project
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

from dataclasses import dataclass
from typing import Any


@dataclass
class JsonRpcProgress:
    """Request progress info.

    :param id: Request ID.
    :type id: int | str
    :param operation: Description of the current task.
    :type operation: str
    :param amount: Progress amount [0-1].
    :type amount: float
    """

    id: int | str
    operation: str
    amount: float

    def __str__(self) -> str:
        return f"[{self.id}] {self.operation}: {100 * self.amount}%"


def deserialize_progress(message: dict[str, Any]) -> JsonRpcProgress:
    params = message["params"]
    return JsonRpcProgress(
        id=params["id"],
        operation=params["operation"],
        amount=params["amount"],
    )
