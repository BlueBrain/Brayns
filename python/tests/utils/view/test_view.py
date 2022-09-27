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

    def test_rotate_around_target(self) -> None:
        euler = brayns.Vector3(0, 180, 0)
        rotation = brayns.Rotation.from_euler(euler, degrees=True)
        test = brayns.View(
            position=brayns.Vector3.zero,
            target=brayns.Vector3.forward,
            up=brayns.Vector3.right,
        )
        test.rotate_around_target(rotation)
        ref = 2 * brayns.Vector3.forward
        self.assertAlmostEqual(test.position.x, ref.x)
        self.assertAlmostEqual(test.position.y, ref.y)
        self.assertAlmostEqual(test.position.z, ref.z)
        self.assertEqual(test.target, brayns.Vector3.forward)
        ref = brayns.Vector3.left
        self.assertAlmostEqual(test.up.x, ref.x)
        self.assertAlmostEqual(test.up.y, ref.y)
        self.assertAlmostEqual(test.up.z, ref.z)


if __name__ == '__main__':
    unittest.main()
