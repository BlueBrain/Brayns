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

import unittest

import brayns


class TestBounds(unittest.TestCase):
    @classmethod
    @property
    def bounds(cls) -> brayns.Bounds:
        return brayns.Bounds(
            min=brayns.Vector3(1, 2, 3),
            max=brayns.Vector3(4, 5, 6),
        )

    @classmethod
    @property
    def corners(cls) -> list[brayns.Vector3]:
        return [
            brayns.Vector3(-1, -1, -1),
            brayns.Vector3(-1, -1, 1),
            brayns.Vector3(-1, 1, -1),
            brayns.Vector3(-1, 1, 1),
            brayns.Vector3(1, -1, -1),
            brayns.Vector3(1, -1, 1),
            brayns.Vector3(1, 1, -1),
            brayns.Vector3(1, 1, 1),
        ]

    def test_of(self) -> None:
        test = brayns.Bounds.of(self.corners)
        ref = brayns.Bounds(-brayns.Vector3.one, brayns.Vector3.one)
        self.assertEqual(test, ref)

    def test_empty(self) -> None:
        test = brayns.Bounds.empty
        self.assertEqual(test.min, brayns.Vector3.zero)
        self.assertEqual(test.max, brayns.Vector3.zero)

    def test_unit(self) -> None:
        test = brayns.Bounds.unit
        self.assertEqual(test.min, -brayns.Vector3.one / 2)
        self.assertEqual(test.max, brayns.Vector3.one / 2)

    def test_contains(self) -> None:
        test = brayns.Bounds(-brayns.Vector3(1, 2, 3), brayns.Vector3(1, 2, 3))
        self.assertTrue(brayns.Vector3(1, 0, 2) in test)
        self.assertTrue(brayns.Vector3.one in test)
        self.assertFalse(brayns.Vector3(2, 2, 3) in test)
        self.assertFalse(-5 * brayns.Vector3.one in test)

    def test_center(self) -> None:
        self.assertEqual(self.bounds.center, brayns.Vector3(2.5, 3.5, 4.5))

    def test_size(self) -> None:
        self.assertEqual(self.bounds.size, brayns.Vector3(3, 3, 3))

    def test_width(self) -> None:
        self.assertEqual(self.bounds.width, 3)

    def test_height(self) -> None:
        self.assertEqual(self.bounds.height, 3)

    def test_depth(self) -> None:
        self.assertEqual(self.bounds.depth, 3)

    def test_corners(self) -> None:
        test = brayns.Bounds(-brayns.Vector3.one, brayns.Vector3.one)
        self.assertEqual(test.corners, self.corners)

    def test_translate(self) -> None:
        translation = brayns.Vector3(7, 8, 9)
        test = self.bounds.translate(translation)
        self.assertEqual(test.min, self.bounds.min + translation)
        self.assertEqual(test.max, self.bounds.max + translation)

    def test_rotate(self) -> None:
        rotation = brayns.CameraRotation.top
        bounds = brayns.Bounds(
            brayns.Vector3.zero,
            brayns.Vector3(1, 2, 3),
        )
        test = bounds.rotate(rotation, center=bounds.center)
        self.assertAlmostEqual(test.width, bounds.width)
        self.assertAlmostEqual(test.height, bounds.depth)
        self.assertAlmostEqual(test.depth, bounds.height)
        self.assertAlmostEqual(test.center.x, bounds.center.x)
        self.assertAlmostEqual(test.center.y, bounds.center.y)
        self.assertAlmostEqual(test.center.z, bounds.center.z)

    def test_rescale(self) -> None:
        scale = brayns.Vector3(1, 2, 3)
        bounds = brayns.Bounds.unit
        test = bounds.rescale(scale)
        self.assertEqual(test.size, scale)
        self.assertEqual(test.center, bounds.center)

    def test_merge_bounds(self) -> None:
        test = brayns.merge_bounds(
            [
                brayns.Bounds(brayns.Vector3(-1, 2, 3), brayns.Vector3(6, 5, 4)),
                brayns.Bounds(brayns.Vector3(1, -2, 3), brayns.Vector3(5, 4, 6)),
                brayns.Bounds(brayns.Vector3(1, 2, -3), brayns.Vector3(4, 6, 5)),
            ]
        )
        ref = brayns.Bounds(
            brayns.Vector3(-1, -2, -3),
            brayns.Vector3(6, 6, 6),
        )
        self.assertEqual(test, ref)

    def test_merge_bounds_empty(self) -> None:
        test = brayns.merge_bounds([])
        ref = brayns.Bounds.empty
        self.assertEqual(test, ref)
