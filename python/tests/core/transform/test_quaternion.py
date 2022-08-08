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

import math
import unittest

from brayns.core.transform.quaternion import Quaternion
from brayns.core.vector.vector3 import Vector3


class TestQuaternion(unittest.TestCase):

    def test_identity(self) -> None:
        self.assertEqual(Quaternion.identity, Quaternion(0, 0, 0, 1))

    def test_mul_scalar(self) -> None:
        values = [1, 2, 3, 4]
        ref = [2 * i for i in values]
        self.assertEqual(Quaternion(*values) * 2, Quaternion(*ref))
        self.assertEqual(2 * Quaternion(*values), Quaternion(*ref))

    def test_mul_other(self) -> None:
        test = Quaternion(1, 2, 3, 4) * Quaternion(5, 6, 7, 8)
        self.assertAlmostEqual(test.x, 24)
        self.assertAlmostEqual(test.y, 48)
        self.assertAlmostEqual(test.z, 48)
        self.assertAlmostEqual(test.w, -6)

    def test_div_scalar(self) -> None:
        values = [1, 2, 3, 4]
        ref = [i / 2 for i in values]
        self.assertEqual(Quaternion(*values) / 2, Quaternion(*ref))
        ref = [2 / i for i in values]
        self.assertEqual(2 / Quaternion(*values), Quaternion(*ref))

    def test_div_other(self) -> None:
        value = Quaternion(1, 2, 3, 4)
        test = value / value
        self.assertAlmostEqual(test.x, 0)
        self.assertAlmostEqual(test.y, 0)
        self.assertAlmostEqual(test.z, 0)
        self.assertAlmostEqual(test.w, 1)

    def test_axis(self) -> None:
        test = Quaternion(1, 2, 3, 4)
        self.assertEqual(test.axis, Vector3(1, 2, 3))

    def test_angle_radians(self) -> None:
        test = Quaternion(1, 0, 0, 0)
        self.assertAlmostEqual(test.angle_radians, math.radians(180))

    def test_angle_degrees(self) -> None:
        test = Quaternion(1, 0, 0, 0)
        self.assertAlmostEqual(test.angle_degrees, 180)

    def test_conjugate(self) -> None:
        test = Quaternion(1, 2, 3, 4)
        ref = Quaternion(-1, -2, -3, 4)
        self.assertEqual(test.conjugate, ref)

    def test_inverse(self) -> None:
        value = Quaternion(1, 2, 3, 4)
        test = value * value.inverse
        self.assertAlmostEqual(test.x, 0)
        self.assertAlmostEqual(test.y, 0)
        self.assertAlmostEqual(test.z, 0)
        self.assertAlmostEqual(test.w, 1)


if __name__ == '__main__':
    unittest.main()
