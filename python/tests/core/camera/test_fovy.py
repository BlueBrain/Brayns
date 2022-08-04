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

from brayns.core.view.fovy import Fovy
from brayns.core.common.bounds import Bounds
from brayns.core.vector.vector3 import Vector3


class TestFovy(unittest.TestCase):

    def test_eq(self) -> None:
        self.assertEqual(Fovy(30), Fovy(30.0))
        self.assertNotEqual(Fovy(31), Fovy(30.0))
        self.assertNotEqual(Fovy(31), None)

    def test_radians(self) -> None:
        angle = math.radians(30)
        fovy = Fovy(angle)
        self.assertAlmostEqual(fovy.radians, angle)
        self.assertAlmostEqual(fovy.degrees, math.degrees(angle))

    def test_degrees(self) -> None:
        angle = 30
        fovy = Fovy(angle, degrees=True)
        self.assertAlmostEqual(fovy.radians, math.radians(angle))
        self.assertAlmostEqual(fovy.degrees, angle)

    def test_get_full_screen_view(self) -> None:
        fovy = Fovy(90, degrees=True)
        target = Bounds(-Vector3.one, Vector3.one)
        test = fovy.get_full_screen_view(target)
        self.assertAlmostEqual(test.position.x, 0)
        self.assertAlmostEqual(test.position.y, 0)
        self.assertAlmostEqual(test.position.z, 2.0)
        self.assertEqual(test.target, Vector3.zero)
        self.assertEqual(test.up, Vector3.up)


if __name__ == '__main__':
    unittest.main()
