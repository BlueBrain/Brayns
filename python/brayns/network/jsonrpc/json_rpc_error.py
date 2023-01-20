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
class JsonRpcError(Exception):
    """Error raised by an instance when a request fails.

    :param id: ID of the request sending the error.
        None for global errors with no requests attached (like invalid JSON).
    :type id: int | str | None
    :param code: Error code.
    :type code: int
    :param message: Error description.
    :type message: str
    :param data: Optional additional error information, defaults to None.
    :type data: Any, optional
    """

    id: int | str | None
    code: int
    message: str
    data: Any = None

    @staticmethod
    def general(message: str) -> JsonRpcError:
        return JsonRpcError(
            id=None,
            code=0,
            message=message,
        )

    def __str__(self) -> str:
        description = self.message
        details = [f"code={self.code}"]
        if self.id is not None:
            details.append(f"request ID={self.id})")
        if self.data is not None:
            details.append(f"data={self.data})")
        detail = ", ".join(details)
        return f"{description} ({detail})"


def deserialize_error(message: dict[str, Any]) -> JsonRpcError:
    error: dict[str, Any] = message["error"]
    return JsonRpcError(
        id=message.get("id"),
        code=error["code"],
        message=error["message"],
        data=error.get("data"),
    )
