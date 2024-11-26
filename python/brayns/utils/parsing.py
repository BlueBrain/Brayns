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
from dataclasses import MISSING, fields, is_dataclass
from enum import Enum
from inspect import get_annotations
from types import NoneType, UnionType
from typing import Any, Literal, Optional, TypeVar, Union, get_args, get_origin, is_typeddict, overload

from .composing import Object, camel_case


def parse_text(data: str) -> Any:
    return json.loads(data)


def parse_binary(data: bytes) -> tuple[Any, bytes]:
    size = len(data)

    if size < 4:
        raise ValueError("Invalid binary frame with header < 4 bytes")

    header = data[:4]

    json_size = int.from_bytes(header, byteorder="little", signed=False)

    if json_size > size - 4:
        raise ValueError(f"Invalid JSON size: {json_size} > {size - 4}")

    json_part = data[4 : 4 + json_size].decode("utf-8")
    message = parse_text(json_part)

    binary_part = data[4 + json_size :]

    return message, binary_part


def parse(data: bytes | str) -> tuple[Any, bytes]:
    if isinstance(data, str):
        return (parse_text(data), b"")

    return parse_binary(data)


T = TypeVar("T")


@overload
def deserialize(value: Any, t: type[T]) -> T: ...


@overload
def deserialize(value: Any, t: Any) -> Any: ...


def deserialize(value: Any, t: Any) -> Any:
    origin = get_origin(t)
    args = get_args(t)

    if origin is None:
        origin = t

    if origin is Literal:
        return _deserialize_literal(value, args)

    if isinstance(origin, TypeVar):
        return value

    if origin in (Any, None, NoneType, bool, int, float, str):
        return _ensure(value, origin)

    if isinstance(t, UnionType) or origin is Union or origin is Optional:
        return _deserialize_union(value, args)

    if is_typeddict(origin):
        return _deserialize_typeddict(value, origin)

    if is_dataclass(origin):
        return _deserialize_dataclass(value, origin)

    if origin is dict:
        return _deserialize_dict(value, args)

    if origin in (list, set):
        return _deserialize_array(value, origin, args)

    if origin is tuple:
        return _deserialize_tuple(value, origin, args)

    if issubclass(origin, Object):
        return origin(deserialize(value, int))

    if issubclass(origin, Enum):
        return _deserialize_enum(value, origin)

    raise TypeError("Unsupported JSON type (internal error)")


def _ensure(value: Any, t: Any) -> Any:
    if t is Any:
        return value

    if t is None:
        t = NoneType

    if t is float and isinstance(value, int):
        return float(value)

    if isinstance(value, t):
        return value

    raise TypeError("Invalid type in JSON response")


def _deserialize_union(value: Any, args: tuple[Any, ...]) -> Any:
    for arg in args:
        try:
            return deserialize(value, arg)
        except TypeError:
            continue

    raise TypeError("Invalid union type in JSON response")


def _deserialize_typeddict(value: Any, origin: Any) -> Any:
    _ensure(value, dict)
    assert isinstance(value, dict)

    result = dict[str, Any]()

    for field_name, field_type in get_annotations(origin).items():
        key = camel_case(field_name)

        missing = key not in value
        required = key in origin.__required_keys__

        if missing and required:
            raise TypeError("Missing required key in JSON response")

        if not missing:
            result[field_name] = deserialize(value[key], field_type)


def _deserialize_enum(value: Any, t: Any) -> Any:
    _ensure(value, str)
    return t(value)


def _deserialize_dataclass(value: Any, origin: Any) -> Any:
    _ensure(value, dict)
    assert isinstance(value, dict)

    kwargs = dict[str, Any]()

    for field in fields(origin):
        key = camel_case(field.name)

        missing = key not in value
        required = field.default is MISSING and field.default_factory is MISSING

        if missing and required:
            raise TypeError("Missing required field in JSON response")

        if not missing:
            kwargs[field.name] = deserialize(value[key], field.type)

    return origin(**kwargs)


def _deserialize_literal(value: Any, args: tuple[Any, ...]) -> Any:
    (expected,) = args

    if value != expected:
        raise TypeError("Invalid literal")

    return value


def _deserialize_dict(value: Any, args: tuple[Any, ...]) -> Any:
    _ensure(value, dict)
    assert isinstance(value, dict)

    key_type, value_type = args

    assert key_type is str

    return {_ensure(key, str): deserialize(item, value_type) for key, item in value.items()}


def _deserialize_array(value: Any, origin: Any, args: tuple[Any, ...]) -> Any:
    _ensure(value, list)
    assert isinstance(value, list)

    (item_type,) = args

    return origin(deserialize(item, item_type) for item in value)


def _deserialize_tuple(value: Any, origin: Any, args: tuple[Any, ...]) -> Any:
    _ensure(value, list)
    assert isinstance(value, list)

    item_types = args

    if len(value) != len(item_types):
        raise TypeError("Invalid item count")

    return origin(deserialize(item, item_type) for item, item_type in zip(value, item_types))
