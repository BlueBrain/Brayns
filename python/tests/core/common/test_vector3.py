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

from brayns.core.common.vector3 import Vector3


class TestVector3(unittest.TestCase):

    def test_full(self) -> None:
        self.assertEqual(Vector3.full(5), Vector3(5, 5, 5))

    def test_unpack(self) -> None:
        test = [1, 2, 3]
        self.assertEqual(Vector3.unpack(test), Vector3(*test))

    def test_zero(self) -> None:
        self.assertEqual(Vector3.zero, Vector3.full(0))

    def test_one(self) -> None:
        self.assertEqual(Vector3.one, Vector3.full(1))

    def test_right(self) -> None:
        self.assertEqual(Vector3.right, Vector3(1, 0, 0))

    def test_left(self) -> None:
        self.assertEqual(Vector3.left, -Vector3.right)

    def test_up(self) -> None:
        self.assertEqual(Vector3.up, Vector3(0, 1, 0))

    def test_down(self) -> None:
        self.assertEqual(Vector3.down, -Vector3.up)

    def test_forward(self) -> None:
        self.assertEqual(Vector3.forward, Vector3(0, 0, 1))

    def test_back(self) -> None:
        self.assertEqual(Vector3.back, -Vector3.forward)

    def test_iter(self) -> None:
        values = [1, 2, 3]
        test = Vector3(*values)
        self.assertEqual(list(test), values)
        self.assertEqual([test.x, test.y, test.z], values)

    def test_neg(self) -> None:
        self.assertEqual(-Vector3(1, 2, 3), Vector3(-1, -2, -3))

    def test_abs(self) -> None:
        self.assertEqual(abs(Vector3(1, -2, -3)), Vector3(1, 2, 3))

    def test_add(self) -> None:
        self.assertEqual(Vector3(1, 2, 3) + Vector3(4, 5, 6), Vector3(5, 7, 9))

    def test_sub(self) -> None:
        self.assertEqual(Vector3(4, 2, 7) - Vector3(1, 2, 3), Vector3(3, 0, 4))

    def test_mul(self) -> None:
        self.assertEqual(Vector3(1, 2, 3) * Vector3(3, 4, 2), Vector3(3, 8, 6))
        self.assertEqual(3 * Vector3(1, 2, 3), Vector3(3, 6, 9))
        self.assertEqual(Vector3(1, 2, 3) * 3, Vector3(3, 6, 9))

    def test_truediv(self) -> None:
        self.assertEqual(
            Vector3(1, 2, 3) / Vector3(1, 2, 4),
            Vector3(1, 1, 0.75)
        )
        self.assertEqual(Vector3(1, 2, 4) / 4, Vector3(0.25, 0.5, 1))
        self.assertEqual(2 / Vector3(1, 2, 4), Vector3(2, 1, 0.5))

    def test_floordiv(self) -> None:
        self.assertEqual(
            Vector3(1, 2, 3) // Vector3(1, 2, 4),
            Vector3(1, 1, 0)
        )
        self.assertEqual(Vector3(1, 2, 4) // 4, Vector3(0, 0, 1))
        self.assertEqual(2 // Vector3(1, 2, 4), Vector3(2, 1, 0))

    def test_mod(self) -> None:
        self.assertEqual(
            Vector3(10, 12, 14) % Vector3(2, 5, 6),
            Vector3(0, 2, 2)
        )
        self.assertEqual(Vector3(3, 4, 5) % 3, Vector3(0, 1, 2))
        self.assertEqual(3 % Vector3(3, 4, 5), Vector3(0, 3, 3))

    def test_pow(self) -> None:
        self.assertEqual(
            Vector3(1, 2, 3) ** Vector3(2, 3, 3),
            Vector3(1, 8, 27)
        )
        self.assertEqual(Vector3(1, 2, 3) ** 2, Vector3(1, 4, 9))
        self.assertEqual(2 ** Vector3(1, 2, 3), Vector3(2, 4, 8))

    def test_square_norm(self) -> None:
        self.assertEqual(Vector3(1, 2, 3).square_norm, 14)

    def test_norm(self) -> None:
        self.assertEqual(Vector3(1, 2, 3).norm, math.sqrt(14))

    def test_normalized(self) -> None:
        value = Vector3(1, 2, 3)
        test = value.normalized
        self.assertAlmostEqual(test.norm, 1)
        test *= value.norm
        self.assertAlmostEqual(test.x, value.x)
        self.assertAlmostEqual(test.y, value.y)
        self.assertAlmostEqual(test.z, value.z)

    def test_dot(self) -> None:
        self.assertEqual(Vector3(1, 2, 3).dot(Vector3(4, 5, 6)), 32)

    def test_cross(self) -> None:
        self.assertEqual(
            Vector3(1, 2, 3).cross(Vector3(4, 5, 6)),
            Vector3(-3, 6, -3)
        )


if __name__ == '__main__':
    unittest.main()
