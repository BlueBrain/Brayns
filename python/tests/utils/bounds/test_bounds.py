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

    def test_from_corners(self) -> None:
        test = brayns.Bounds.from_corners(self.corners)
        ref = brayns.Bounds(-brayns.Vector3.one, brayns.Vector3.one)
        self.assertEqual(test, ref)

    def test_empty(self) -> None:
        test = brayns.Bounds.empty
        self.assertEqual(test.min, brayns.Vector3.zero)
        self.assertEqual(test.max, brayns.Vector3.zero)

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


if __name__ == '__main__':
    unittest.main()
