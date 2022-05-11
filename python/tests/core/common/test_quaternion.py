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

import unittest

from brayns.core.common.quaternion import Quaternion
from brayns.core.common.vector3 import Vector3


class TestQuaternion(unittest.TestCase):

    def test_from_euler(self) -> None:
        test = Quaternion.from_euler(Vector3(34, -22, -80), degrees=True)
        self.assertAlmostEqual(test.norm, 1)
        self.assertAlmostEqual(test.x, 0.10256431)
        self.assertAlmostEqual(test.y, -0.32426137)
        self.assertAlmostEqual(test.z, -0.56067163)
        self.assertAlmostEqual(test.w, 0.75497182)

    def test_from_axis_angle(self) -> None:
        axis = Vector3(1, 2, 3)
        angle = 30
        test = Quaternion.from_axis_angle(axis, angle, degrees=True)
        self.assertAlmostEqual(test.norm, 1)
        self.assertAlmostEqual(test.x, 0.0691723)
        self.assertAlmostEqual(test.y, 0.1383446)
        self.assertAlmostEqual(test.z, 0.2075169)
        self.assertAlmostEqual(test.w, 0.96592583)

    def test_from_vector(self) -> None:
        vector = Vector3(1, 2, 3)
        test = Quaternion.from_vector(vector)
        self.assertEqual(test, Quaternion(1, 2, 3, 0))

    def test_unpack(self) -> None:
        values = [1, 2, 3, 4]
        self.assertEqual(Quaternion.unpack(values), Quaternion(*values))

    def test_identity(self) -> None:
        self.assertEqual(Quaternion.identity, Quaternion(0, 0, 0, 1))

    def test_iter(self) -> None:
        values = [1, 2, 3, 4]
        test = Quaternion(*values)
        self.assertEqual(list(test), values)

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

    def test_vector(self) -> None:
        test = Quaternion(1, 2, 3, 4)
        self.assertEqual(test.vector, Vector3(1, 2, 3))

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

    def test_rotate(self) -> None:
        rotation = Quaternion.from_euler(Vector3(22, 35, 68), degrees=True)
        value = Vector3(1, 2, 3)
        test = rotation.rotate(value)
        self.assertAlmostEqual(test.x, 0.3881471)
        self.assertAlmostEqual(test.y, 2.91087149)
        self.assertAlmostEqual(test.z, 2.31865673)

    def test_rotate_center(self) -> None:
        rotation = Quaternion.from_euler(Vector3(22, 35, 68), degrees=True)
        value = Vector3(1, 2, 3)
        center = Vector3(4, 5, 6)
        test = rotation.rotate(value, center)
        self.assertAlmostEqual(test.x, 3.77731325)
        self.assertAlmostEqual(test.y, 0.02357039)
        self.assertAlmostEqual(test.z, 4.52163639)


if __name__ == '__main__':
    unittest.main()
