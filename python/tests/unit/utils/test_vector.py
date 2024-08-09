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

import math

import pytest
from brayns import Vector2, Vector3, Vector4


def test_component_count() -> None:
    assert Vector2.component_count() == 2
    assert Vector3.component_count() == 3
    assert Vector4.component_count() == 4


def test_full() -> None:
    assert Vector2.zeros() == Vector2(0, 0)
    assert Vector3.zeros() == Vector3(0, 0, 0)
    assert Vector4.zeros() == Vector4(0, 0, 0, 0)

    assert Vector2.ones() == Vector2(1, 1)
    assert Vector3.ones() == Vector3(1, 1, 1)
    assert Vector4.ones() == Vector4(1, 1, 1, 1)

    value = 4.5
    assert Vector2.full(value) == Vector2(value, value)
    assert Vector3.full(value) == Vector3(value, value, value)
    assert Vector4.full(value) == Vector4(value, value, value, value)


def test_componentwise() -> None:
    values = [Vector3(1, -2, 3), Vector3(-1, 2, -3)]

    assert Vector3.componentwise_min(values) == Vector3(-1, -2, -3)
    assert Vector3.componentwise_max(values) == Vector3(1, 2, 3)


def test_conversion() -> None:
    vector2 = Vector2(1, 2)
    vector3 = Vector3(*vector2, 3)
    vector4 = Vector4(*vector3, 4)

    assert vector3 == Vector3(1, 2, 3)
    assert vector4 == Vector4(1, 2, 3, 4)
    assert vector3.xy == vector2
    assert vector4.xyz == vector3

    assert vector2.x == 1
    assert vector3.x == 1
    assert vector4.x == 1

    assert vector2.y == 2
    assert vector3.y == 2
    assert vector4.y == 2

    assert vector3.z == 3
    assert vector4.z == 3

    assert vector4.w == 4


def test_iter() -> None:
    values = [1, 2, 3]
    test = Vector3.unpack(values)

    assert test == Vector3(1, 2, 3)
    assert list(test) == values

    x, y, z = test
    assert x == 1
    assert y == 2
    assert z == 3

    assert len(test) == 3
    assert test.component_count() == 3


def test_getitem() -> None:
    test = Vector3(1, 2, 3)

    assert test[0] == 1
    assert test[1] == 2
    assert test[2] == 3


def test_compare() -> None:
    test = Vector3(1, 2, 3)

    assert test == Vector3(1, 2, 3)
    assert test != Vector3(0, 2, 3)
    assert test < Vector3(1, 3, 3)
    assert test < Vector3(3, 1, 2)
    assert test <= Vector3(3, 1, 2)
    assert test > Vector3(0, 1, 2)
    assert test >= Vector3(0, 1, 2)


def test_pos_neg() -> None:
    test = Vector3(1, 2, 3)

    assert +test == Vector3(1, 2, 3)
    assert -test == Vector3(-1, -2, -3)


def test_abs() -> None:
    assert abs(Vector3(1, -2, 0)) == Vector3(1, 2, 0)


def test_add() -> None:
    assert Vector3(1, 2, 3) + Vector3(2, 3, 4) == Vector3(3, 5, 7)
    assert Vector3(1, 2, 3) + 1 == Vector3(2, 3, 4)
    assert 1 + Vector3(1, 2, 3) == Vector3(2, 3, 4)


def test_sub() -> None:
    assert Vector3(2, 3, 4) - Vector3(1, 3, 2) == Vector3(1, 0, 2)
    assert Vector3(1, 2, 3) - 1 == Vector3(0, 1, 2)
    assert 1 - Vector3(1, 2, 3) == Vector3(0, -1, -2)


def test_mul() -> None:
    assert Vector3(1, 2, 3) * Vector3(2, 3, 4) == Vector3(2, 6, 12)
    assert Vector3(1, 2, 3) * 2 == Vector3(2, 4, 6)
    assert 2 * Vector3(1, 2, 3) == Vector3(2, 4, 6)


def test_div() -> None:
    assert Vector3(1, 2, 3) / Vector3(2, 5, 4) == Vector3(0.5, 0.4, 0.75)
    assert Vector3(1, 2, 3) / 2 == Vector3(0.5, 1, 1.5)
    assert 2 / Vector3(1, 2, 4) == Vector3(2, 1, 0.5)

    assert Vector3(1, 10, 3) // Vector3(2, 5, 3) == Vector3(0, 2, 1)
    assert Vector3(1, 2, 3) // 3 == Vector3(0, 0, 1)
    assert 4 // Vector3(1, 3, 5) == Vector3(4, 1, 0)


def test_mod() -> None:
    assert Vector3(1, 5, 11) % Vector3(1, 2, 3) == Vector3(0, 1, 2)
    assert Vector3(1, 2, 3) % 3 == Vector3(1, 2, 0)
    assert 3 % Vector3(1, 2, 3) == Vector3(0, 1, 0)


def test_pow() -> None:
    assert Vector3(1, 2, 3) ** Vector3(1, 2, 3) == Vector3(1, 4, 27)
    assert Vector3(1, 2, 3) ** 3 == Vector3(1, 8, 27)
    assert 3 ** Vector3(1, 2, 3) == Vector3(3, 9, 27)


def test_norm() -> None:
    test = Vector3(1, 2, 3)

    assert test.square_norm == 14
    assert test.norm == math.sqrt(14)
    assert test.normalized.norm == pytest.approx(1)


def test_dot() -> None:
    assert Vector3(1, 2, 3).dot(Vector3(4, 5, 6)) == 32


def test_cross() -> None:
    assert Vector3(1, 2, 3).cross(Vector3(4, 5, 6)) == Vector3(-3, 6, -3)


def test_reduce() -> None:
    assert Vector3(1, 2, 3).reduce_multiply() == 6
    assert Vector3(1, -2, 3).reduce_multiply() == -6
    assert Vector3(0, -2, 3).reduce_multiply() == 0
