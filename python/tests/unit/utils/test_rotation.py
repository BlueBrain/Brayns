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
from brayns import Rotation, euler, axis_angle, get_rotation_between, Quaternion, Vector3


def test_quaternion() -> None:
    quaternion = Quaternion(1, 2, 3, 4)
    test = Rotation(quaternion)

    assert test.quaternion == quaternion.normalized

    identity = Rotation()

    assert identity.quaternion == Quaternion(0, 0, 0, 1)


def test_euler() -> None:
    angles = Vector3(34, -22, -80)

    test = euler(*angles, degrees=True)
    quaternion = test.quaternion

    assert quaternion.norm == pytest.approx(1)
    assert quaternion.x == pytest.approx(0.10256431)
    assert quaternion.y == pytest.approx(-0.32426137)
    assert quaternion.z == pytest.approx(-0.56067163)
    assert quaternion.w == pytest.approx(0.75497182)

    degrees = test.euler_degrees

    assert degrees.x == pytest.approx(angles.x)
    assert degrees.y == pytest.approx(angles.y)
    assert degrees.z == pytest.approx(angles.z)

    radians = test.euler_radians

    assert radians.x == pytest.approx(math.radians(angles.x))
    assert radians.y == pytest.approx(math.radians(angles.y))
    assert radians.z == pytest.approx(math.radians(angles.z))


def test_axis_angle() -> None:
    axis = Vector3(1, 2, 3)
    angle = 30

    test = axis_angle(*axis, angle, degrees=True)
    quaternion = test.quaternion

    assert quaternion.norm == pytest.approx(1)

    assert quaternion.x == pytest.approx(0.0691723)
    assert quaternion.y == pytest.approx(0.1383446)
    assert quaternion.z == pytest.approx(0.2075169)
    assert quaternion.w == pytest.approx(0.96592583)

    assert test.axis.x == pytest.approx(axis.normalized.x)
    assert test.axis.y == pytest.approx(axis.normalized.y)
    assert test.axis.z == pytest.approx(axis.normalized.z)

    assert test.angle_degrees == pytest.approx(angle)
    assert test.angle_radians == pytest.approx(math.radians(angle))


def test_rotation_between() -> None:
    u = Vector3(1, 0, 0)
    v = Vector3(1, 1, 0)

    rotation = get_rotation_between(u, v)
    test = rotation.euler_degrees

    assert test.x == pytest.approx(0)
    assert test.y == pytest.approx(0)
    assert test.z == pytest.approx(45)

    u = Vector3(1, 0, 0)
    v = Vector3(1, 0, 0)

    identity = get_rotation_between(u, v)

    assert identity == Rotation()

    u = Vector3(1, 0, 0)
    v = Vector3(-1, 0, 0)

    opposite = get_rotation_between(u, v)
    back = opposite.apply(u)

    assert back.x == pytest.approx(v.x)
    assert back.y == pytest.approx(v.y)
    assert back.z == pytest.approx(v.z)


def test_inverse() -> None:
    quaternion = Quaternion(1, 2, 3, 4).normalized
    rotation = Rotation(quaternion)

    assert rotation.inverse.quaternion == quaternion.inverse


def test_then() -> None:
    axis = Vector3(0, 1, 0)
    angle = 30

    r1 = axis_angle(*axis, angle, degrees=True)
    r2 = axis_angle(*axis, angle, degrees=True)

    combined = r1.then(r2)

    assert combined.angle_degrees == pytest.approx(2 * angle)
    assert combined.axis.x == pytest.approx(axis.x)
    assert combined.axis.y == pytest.approx(axis.y)
    assert combined.axis.z == pytest.approx(axis.z)


def test_apply() -> None:
    rotation = euler(22, 35, 68, degrees=True)
    value = Vector3(1, 2, 3)

    test = rotation.apply(value)

    assert test.x == pytest.approx(0.3881471)
    assert test.y == pytest.approx(2.91087149)
    assert test.z == pytest.approx(2.31865673)

    center = Vector3(4, 5, 6)
    test = rotation.apply(value, center)

    assert test.x == pytest.approx(3.77731325)
    assert test.y == pytest.approx(0.02357039)
    assert test.z == pytest.approx(4.52163639)
