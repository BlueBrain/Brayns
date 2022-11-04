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


class TestTransform(unittest.TestCase):

    def test_rotate(self) -> None:
        rotation = brayns.euler(0, 0, 90, degrees=True)
        center = brayns.Vector3(0, 1, 0)
        test = brayns.Transform.rotate(rotation, center)
        self.assertAlmostEqual(test.translation.x, 1)
        self.assertAlmostEqual(test.translation.y, 1)
        self.assertAlmostEqual(test.translation.z, 0)
        self.assertEqual(test.rotation, rotation)
        self.assertEqual(test.scale, brayns.Vector3.one)

    def test_identity(self) -> None:
        test = brayns.Transform.identity
        self.assertEqual(test.translation, brayns.Vector3.zero)
        self.assertEqual(test.rotation, brayns.Rotation.identity)
        self.assertEqual(test.scale, brayns.Vector3.one)

    def test_apply(self) -> None:
        transform = brayns.Transform(
            translation=brayns.Vector3.one,
            rotation=brayns.euler(0, 0, 90, degrees=True),
            scale=2 * brayns.Vector3.one,
        )
        value = brayns.Vector3.one
        test = transform.apply(value)
        self.assertAlmostEqual(test.x, -1)
        self.assertAlmostEqual(test.y, 3)
        self.assertAlmostEqual(test.z, 3)

    def test_then(self) -> None:
        first = brayns.Transform(
            translation=brayns.Vector3.one,
            rotation=brayns.euler(0, 0, 90, degrees=True),
            scale=2 * brayns.Vector3.one,
        )
        second = brayns.Transform(
            translation=2 * brayns.Vector3.one,
            rotation=brayns.euler(0, 90, 0, degrees=True),
            scale=3 * brayns.Vector3.one,
        )
        rotation = first.rotation.then(second.rotation)
        test = first.then(second)
        self.assertEqual(test.translation, 3 * brayns.Vector3.one)
        self.assertEqual(test.rotation, rotation)
        self.assertEqual(test.scale, 6 * brayns.Vector3.one)


if __name__ == '__main__':
    unittest.main()
