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


class TestVector(unittest.TestCase):
    def test_from_vector2(self) -> None:
        vector2 = brayns.Vector2(1, 2)
        test = brayns.Vector3.from_vector2(vector2, 3)
        self.assertEqual(test, brayns.Vector3(1, 2, 3))

    def test_unpack(self) -> None:
        test = [1, 2, 3]
        self.assertEqual(brayns.Vector3.unpack(test), brayns.Vector3(*test))

    def test_zero(self) -> None:
        self.assertEqual(brayns.Vector3.zero, brayns.Vector3(0, 0, 0))

    def test_one(self) -> None:
        self.assertEqual(brayns.Vector3.one, brayns.Vector3(1, 1, 1))

    def test_str(self) -> None:
        test = brayns.Vector3(1, 2, 3)
        self.assertEqual(str(test), "Vector3(1, 2, 3)")
        self.assertEqual(repr(test), str(test))

    def test_iter(self) -> None:
        values = [1, 2, 3]
        test = brayns.Vector3(*values)
        self.assertEqual(list(test), values)
        self.assertEqual([test.x, test.y, test.z], values)

    def test_len(self) -> None:
        test = brayns.Vector3(1, 2, 3)
        self.assertEqual(len(test), 3)

    def test_getitem(self) -> None:
        test = brayns.Vector3(1, 2, 3)
        self.assertEqual(test[0], 1)
        self.assertEqual(test[1], 2)
        self.assertEqual(test[2], 3)
        with self.assertRaises(IndexError):
            test[3]

    def test_compare(self) -> None:
        test = brayns.Vector3(1, 2, 3)
        self.assertEqual(test, brayns.Vector3(1, 2, 3))
        self.assertNotEqual(test, brayns.Vector3(1, 3, 3))
        self.assertLess(test, brayns.Vector3(3, 1, 2))
        self.assertLessEqual(test, brayns.Vector3(3, 1, 2))
        self.assertGreater(test, brayns.Vector3(0, 1, 2))
        self.assertGreaterEqual(test, brayns.Vector3(0, 1, 2))

    def test_neg(self) -> None:
        self.assertEqual(-brayns.Vector3(1, 2, 3), brayns.Vector3(-1, -2, -3))

    def test_pos(self) -> None:
        self.assertEqual(+brayns.Vector3(1, 2, 3), brayns.Vector3(1, 2, 3))

    def test_abs(self) -> None:
        test = abs(brayns.Vector3(1, -2, -3))
        ref = brayns.Vector3(1, 2, 3)
        self.assertEqual(test, ref)

    def test_add(self) -> None:
        test = brayns.Vector3(1, 2, 3) + brayns.Vector3(4, 5, 6)
        ref = brayns.Vector3(5, 7, 9)
        self.assertEqual(test, ref)

    def test_add_scalar(self) -> None:
        test = brayns.Vector3(1, 2, 3) + 1
        ref = brayns.Vector3(2, 3, 4)
        self.assertEqual(test, ref)
        test = 1 + brayns.Vector3(1, 2, 3)
        self.assertEqual(test, ref)

    def test_sub(self) -> None:
        test = brayns.Vector3(4, 2, 7) - brayns.Vector3(1, 2, 3)
        ref = brayns.Vector3(3, 0, 4)
        self.assertEqual(test, ref)

    def test_sub_scalar(self) -> None:
        test = brayns.Vector3(1, 2, 3) - 1
        ref = brayns.Vector3(0, 1, 2)
        self.assertEqual(test, ref)
        test = brayns.Vector3(1, 2, 3) - 1
        self.assertEqual(test, ref)

    def test_mul_scalar(self) -> None:
        test = brayns.Vector3(1, 2, 3)
        ref = brayns.Vector3.unpack(i * 3 for i in test)
        self.assertEqual(test * 3, ref)
        self.assertEqual(3 * test, ref)

    def test_mul_other(self) -> None:
        test = brayns.Vector3(1, 2, 3) * brayns.Vector3(4, 5, 6)
        ref = brayns.Vector3(4, 10, 18)
        self.assertEqual(test, ref)

    def test_truediv_scalar(self) -> None:
        test = brayns.Vector3(1, 2, 3)
        ref = brayns.Vector3.unpack(i / 3 for i in test)
        self.assertEqual(test / 3, ref)
        ref = brayns.Vector3.unpack(3 / i for i in test)
        self.assertEqual(3 / test, ref)

    def test_truediv_other(self) -> None:
        test = brayns.Vector3(1, 2, 3) / brayns.Vector3(4, 5, 6)
        ref = brayns.Vector3(1 / 4, 2 / 5, 3 / 6)
        self.assertEqual(test, ref)

    def test_floordiv_scalar(self) -> None:
        test = brayns.Vector3(4, 6, 10)
        ref = brayns.Vector3.unpack(i // 3 for i in test)
        self.assertEqual(test // 3, ref)
        ref = brayns.Vector3.unpack(3 // i for i in test)
        self.assertEqual(3 // test, ref)

    def test_floordiv_other(self) -> None:
        test = brayns.Vector3(1, 5, 10) // brayns.Vector3(1, 2, 3)
        ref = brayns.Vector3(1, 2, 3)
        self.assertEqual(test, ref)

    def test_mod_scalar(self) -> None:
        test = brayns.Vector3(1, 2, 3)
        ref = brayns.Vector3.unpack(i % 3 for i in test)
        self.assertEqual(test % 3, ref)
        ref = brayns.Vector3.unpack(3 % i for i in test)
        self.assertEqual(3 % test, ref)

    def test_mod_other(self) -> None:
        test = brayns.Vector3(1, 5, 11) % brayns.Vector3(1, 2, 3)
        ref = brayns.Vector3(0, 1, 2)
        self.assertEqual(test, ref)

    def test_pow_scalar(self) -> None:
        test = brayns.Vector3(1, 2, 3)
        ref = brayns.Vector3.unpack(i**3 for i in test)
        self.assertEqual(test**3, ref)
        ref = brayns.Vector3.unpack(3**i for i in test)
        self.assertEqual(3**test, ref)

    def test_pow_other(self) -> None:
        test = brayns.Vector3(1, 2, 3) ** brayns.Vector3(4, 5, 6)
        ref = brayns.Vector3(1, 32, 729)
        self.assertEqual(test, ref)

    def test_square_norm(self) -> None:
        self.assertEqual(brayns.Vector3(1, 2, 3).square_norm, 14)

    def test_norm(self) -> None:
        self.assertEqual(brayns.Vector3(1, 2, 3).norm, math.sqrt(14))

    def test_xy(self) -> None:
        test = brayns.Vector3(1, 2, 3)
        self.assertEqual(test.xy, brayns.Vector2(1, 2))

    def test_xz(self) -> None:
        test = brayns.Vector3(1, 2, 3)
        self.assertEqual(test.xz, brayns.Vector2(1, 3))

    def test_yz(self) -> None:
        test = brayns.Vector3(1, 2, 3)
        self.assertEqual(test.yz, brayns.Vector2(2, 3))

    def test_normalized(self) -> None:
        value = brayns.Vector3(1, 2, 3)
        test = value.normalized
        self.assertAlmostEqual(test.norm, 1)
        test *= value.norm
        self.assertAlmostEqual(test.x, value.x)
        self.assertAlmostEqual(test.y, value.y)
        self.assertAlmostEqual(test.z, value.z)

    def test_dot(self) -> None:
        left = brayns.Vector3(1, 2, 3)
        right = brayns.Vector3(4, 5, 6)
        self.assertEqual(left.dot(right), 32)

    def test_cross(self) -> None:
        left = brayns.Vector3(1, 2, 3)
        right = brayns.Vector3(4, 5, 6)
        self.assertEqual(left.cross(right), brayns.Vector3(-3, 6, -3))

    def test_componentwise_min(self) -> None:
        test = brayns.componentwise_min(
            [
                brayns.Vector3(-3, 2, 1),
                brayns.Vector3(-1, -4, 2),
                brayns.Vector3(-2, 1, 3),
            ]
        )
        ref = brayns.Vector3(-3, -4, 1)
        self.assertEqual(test, ref)

    def test_componentwise_min_empty(self) -> None:
        test = brayns.componentwise_min([])
        ref = brayns.Vector3.zero
        self.assertEqual(test, ref)

    def test_componentwise_max(self) -> None:
        test = brayns.componentwise_max(
            [
                brayns.Vector3(-3, -2, 1),
                brayns.Vector3(1, 4, 2),
                brayns.Vector3(-2, 1, 3),
            ]
        )
        ref = brayns.Vector3(1, 4, 3)
        self.assertEqual(test, ref)

    def test_componentwise_max_empty(self) -> None:
        test = brayns.componentwise_max([])
        ref = brayns.Vector3.zero
        self.assertEqual(test, ref)
