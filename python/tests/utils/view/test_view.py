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


class TestView(unittest.TestCase):

    def test_vector(self) -> None:
        test = brayns.View(
            position=brayns.Vector3.zero,
            target=brayns.Vector3.one,
        )
        self.assertEqual(test.vector, brayns.Vector3.one)

    def test_direction(self) -> None:
        test = brayns.View()
        self.assertEqual(test.direction, test.vector.normalized)

    def test_distance(self) -> None:
        test = brayns.View()
        self.assertEqual(test.distance, test.vector.norm)
        test.distance = 3
        self.assertEqual(test.distance, 3)

    def test_translate(self) -> None:
        translation = brayns.Vector3(1, 2, 3)
        ref = brayns.View()
        test = ref.translate(translation)
        self.assertEqual(test.position, ref.position + translation)
        self.assertEqual(test.target, ref.target + translation)
        self.assertEqual(test.up, ref.up)

    def test_rotate_around_target(self) -> None:
        rotation = brayns.euler(0, 180, 0, degrees=True)
        test = brayns.View(
            position=brayns.Vector3.zero,
            target=brayns.Axis.front,
            up=brayns.Axis.right,
        )
        test = test.rotate_around_target(rotation)
        ref = 2 * brayns.Axis.front
        self.assertAlmostEqual(test.position.x, ref.x)
        self.assertAlmostEqual(test.position.y, ref.y)
        self.assertAlmostEqual(test.position.z, ref.z)
        self.assertEqual(test.target, brayns.Axis.front)
        ref = brayns.Axis.left
        self.assertAlmostEqual(test.up.x, ref.x)
        self.assertAlmostEqual(test.up.y, ref.y)
        self.assertAlmostEqual(test.up.z, ref.z)


if __name__ == '__main__':
    unittest.main()
