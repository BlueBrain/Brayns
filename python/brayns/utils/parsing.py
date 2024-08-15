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

from types import UnionType
from typing import Any, cast, get_args, get_origin


def has_type(value: Any, t: Any) -> bool:
    if t is Any:
        return True

    if t is None:
        return value is None

    if t is float and isinstance(value, int):
        return True

    origin = get_origin(t)

    if origin is UnionType:
        args = get_args(t)
        return any(has_type(value, arg) for arg in args)

    if origin is None:
        origin = t

    if not isinstance(value, origin):
        return False

    if origin is list:
        items = cast(list, value)
        (arg,) = get_args(t)
        return all(has_type(item, arg) for item in items)

    if origin is dict:
        object = cast(dict, value)
        keytype, valuetype = get_args(t)
        return all(has_type(key, keytype) and has_type(item, valuetype) for key, item in object.items())

    return True


def check_type(value: Any, t: Any) -> None:
    if not has_type(value, t):
        raise TypeError("Invalid type in JSON-RPC message")


def try_get(message: dict[str, Any], key: str, t: Any, default: Any = None) -> Any:
    value = message.get(key, default)

    check_type(value, t)

    return value


def get(message: dict[str, Any], key: str, t: Any) -> Any:
    if key not in message:
        raise KeyError(f"Missing mandatory key in JSON-RPC message: '{key}'")

    value = message[key]
    check_type(value, t)

    return value


def get_tuple(message: dict[str, Any], key: str, t: Any, item_count: int) -> Any:
    value = get(message, key, list[t])

    if len(value) != item_count:
        raise ValueError(f"Expected {item_count} items for '{key}'")

    return value
