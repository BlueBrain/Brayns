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
from typing import Any, TypeVar, cast, get_args, get_origin

from .box import Box, Box1, Box2, Box3
from .quaternion import Quaternion
from .vector import Vector2, Vector3, Vector4


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

    if value is default:
        return value

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


T = TypeVar("T", Vector2, Vector3, Vector4, Quaternion)


def deserialize_vector(message: dict[str, Any], key: str, t: type[T]) -> T:
    value = get_tuple(message, key, float, t.component_count())
    return t(*value)


def serialize_box(value: Box2 | Box3) -> dict[str, Any]:
    return {
        "min": value.min,
        "max": value.max,
    }


U = TypeVar("U", bound=Box)


def deserialize_box(message: dict[str, Any], t: type[U]) -> U:
    return t(
        min=deserialize_vector(message, "min", t.vector_type()),
        max=deserialize_vector(message, "max", t.vector_type()),
    )


def serialize_box1(value: Box1) -> dict[str, Any]:
    return {
        "min": value.min,
        "max": value.max,
    }


def deserialize_box1(message: dict[str, Any]) -> Box1:
    return Box1(
        min=get(message, "min", float),
        max=get(message, "max", float),
    )
