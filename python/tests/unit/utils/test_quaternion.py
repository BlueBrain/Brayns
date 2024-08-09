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
from brayns import Quaternion, Vector3


def test_identity() -> None:
    assert Quaternion() == Quaternion(0, 0, 0, 1)


def test_properties() -> None:
    test = Quaternion(1, 2, 3, 4)

    assert test.x == 1
    assert test.y == 2
    assert test.z == 3
    assert test.w == 4


def test_mul() -> None:
    assert 2 * Quaternion(1, 2, 3, 4) == Quaternion(2, 4, 6, 8)
    assert Quaternion(1, 2, 3, 4) * 2 == Quaternion(2, 4, 6, 8)

    assert Quaternion(1, 2, 3, 4) * Quaternion(5, 6, 7, 8) == Quaternion(24, 48, 48, -6)


def test_div() -> None:
    assert Quaternion(1, 2, 3, 4) / 2 == Quaternion(0.5, 1, 1.5, 2)
    assert 2 / Quaternion(1, 2, 4, 5) == Quaternion(2, 1, 0.5, 0.4)

    test = Quaternion(1, 2, 3, 4)
    assert test * test.inverse == Quaternion()
    assert test / test == Quaternion()


def test_angle_axis() -> None:
    test = Quaternion(1, 2, 3, 4)

    assert test.xyz == Vector3(1, 2, 3)
    assert test.axis == test.xyz.normalized

    flip = Quaternion(1, 0, 0, 0)

    assert flip.angle_degrees == pytest.approx(180)
    assert flip.angle_radians == pytest.approx(math.radians(180))


def test_conjugate() -> None:
    assert Quaternion(1, 2, 3, 4).conjugate == Quaternion(-1, -2, -3, 4)
