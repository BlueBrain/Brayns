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

from typing import Any

from .deserialize_schema import deserialize_schema
from .entrypoint import Entrypoint
from .json_schema import JsonSchema


def deserialize_entrypoint(message: dict[str, Any]) -> Entrypoint:
    return Entrypoint(
        method=message['title'],
        description=message['description'],
        plugin=message['plugin'],
        asynchronous=message['async'],
        params=_deserialize_schema(message, 'params'),
        result=_deserialize_schema(message, 'returns'),
    )


def _deserialize_schema(message: dict[str, Any], key: str) -> JsonSchema | None:
    value = message.get(key)
    if value is None:
        return None
    return deserialize_schema(value)
