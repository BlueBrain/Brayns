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
import unittest

import brayns


class TestQuaternion(unittest.TestCase):
    def test_identity(self) -> None:
        test = brayns.Quaternion.identity
        ref = brayns.Quaternion(0, 0, 0, 1)
        self.assertEqual(test, ref)

    def test_mul_scalar(self) -> None:
        test = brayns.Quaternion(1, 2, 3, 4)
        ref = brayns.Quaternion.unpack(2 * i for i in test)
        self.assertEqual(test * 2, ref)
        self.assertEqual(2 * test, ref)

    def test_mul_other(self) -> None:
        test = brayns.Quaternion(1, 2, 3, 4) * brayns.Quaternion(5, 6, 7, 8)
        ref = brayns.Quaternion(24, 48, 48, -6)
        self.assertEqual(test, ref)

    def test_div_scalar(self) -> None:
        test = brayns.Quaternion(1, 2, 3, 4)
        ref = brayns.Quaternion.unpack(i / 2 for i in test)
        self.assertEqual(test / 2, ref)
        ref = brayns.Quaternion.unpack(2 / i for i in test)
        self.assertEqual(2 / test, ref)

    def test_div_other(self) -> None:
        quaternion = brayns.Quaternion(1, 2, 3, 4)
        test = quaternion / quaternion
        self.assertEqual(test, brayns.Quaternion.identity)

    def test_xyz(self) -> None:
        test = brayns.Quaternion(1, 2, 3, 4)
        self.assertEqual(test.xyz, brayns.Vector3(1, 2, 3))

    def test_axis(self) -> None:
        test = brayns.Quaternion(1, 2, 3, 4)
        self.assertEqual(test.axis, test.xyz.normalized)

    def test_angle_radians(self) -> None:
        test = brayns.Quaternion(1, 0, 0, 0)
        self.assertAlmostEqual(test.angle_radians, math.radians(180))

    def test_angle_degrees(self) -> None:
        test = brayns.Quaternion(1, 0, 0, 0)
        self.assertAlmostEqual(test.angle_degrees, 180)

    def test_conjugate(self) -> None:
        test = brayns.Quaternion(1, 2, 3, 4)
        ref = brayns.Quaternion(-1, -2, -3, 4)
        self.assertEqual(test.conjugate, ref)

    def test_inverse(self) -> None:
        test = brayns.Quaternion(1, 2, 3, 4)
        self.assertEqual(test * test.inverse, brayns.Quaternion.identity)
