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

from brayns import Vector3, View, X, Y, Z, axis_angle


def test_directions() -> None:
    front = View()

    assert front.position == Vector3()
    assert front.direction == -Z
    assert front.up == Y

    assert front.right == X
    assert front.left == -X
    assert front.real_up == Y
    assert front.down == -Y
    assert front.forward == -Z
    assert front.back == Z


def test_rotate() -> None:
    front = View()

    axis = -X
    angle = math.radians(90)
    rotation = axis_angle(*axis, angle)

    top = front.rotate(rotation)

    assert top.position.x == pytest.approx(0)
    assert top.position.y == pytest.approx(0)
    assert top.position.z == pytest.approx(0)

    assert top.direction.x == pytest.approx(0)
    assert top.direction.y == pytest.approx(-1)
    assert top.direction.z == pytest.approx(0)

    assert top.up.x == pytest.approx(0)
    assert top.up.y == pytest.approx(0)
    assert top.up.z == pytest.approx(-1)


def test_rotation_to() -> None:
    source = View()
    destination = View(direction=-Y, up=-Z)

    rotation = source.get_rotation_to(destination)

    test = source.rotate(rotation)

    assert test.position.x == pytest.approx(destination.position.x)
    assert test.position.y == pytest.approx(destination.position.y)
    assert test.position.z == pytest.approx(destination.position.z)

    assert test.direction.x == pytest.approx(destination.direction.x)
    assert test.direction.y == pytest.approx(destination.direction.y)
    assert test.direction.z == pytest.approx(destination.direction.z)

    assert test.up.x == pytest.approx(destination.up.x)
    assert test.up.y == pytest.approx(destination.up.y)
    assert test.up.z == pytest.approx(destination.up.z)
    assert test.up.z == pytest.approx(destination.up.z)
    assert test.up.z == pytest.approx(destination.up.z)