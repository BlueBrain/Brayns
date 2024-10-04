# Copyright (c) 2015-2024 EPFL/Blue Brain Project
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
from dataclasses import is_dataclass
from enum import Enum
from itertools import chain, islice
from types import NoneType
from typing import Any, NamedTuple


class Object(NamedTuple):
    id: int


def camel_case(value: str) -> str:
    parts = value.split("_")
    first = parts[0]
    rest = (part.capitalize() for part in islice(parts, 1, None))
    return "".join(value for value in chain(first, rest))


def compose_text(message: Any) -> str:
    return json.dumps(message, separators=(",", ":"))


def compose_binary(message: Any, binary: bytes) -> bytes:
    text = compose_text(message)
    json_part = text.encode()

    header = len(json_part).to_bytes(4, byteorder="little", signed=False)

    return header + json_part + binary


def compose(message: Any, binary: bytes = b"") -> bytes | str:
    return compose_binary(message, binary) if binary else compose_text(message)


def serialize(value: Any) -> Any:
    if isinstance(value, NoneType | bool | int | float | str):
        return value

    if isinstance(value, Object):
        return value.id

    if isinstance(value, Enum):
        return value.value

    if is_dataclass(value):
        return serialize(vars(value))

    if isinstance(value, dict):
        return {camel_case(key): serialize(item) for key, item in value.items()}

    if isinstance(value, tuple | list | set):
        return [serialize(item) for item in value]

    raise TypeError("Unsupported type in JSON request")
