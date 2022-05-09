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

from brayns.core.camera.perspective_camera import PerspectiveCamera
from brayns.core.common.bounds import Bounds
from brayns.core.common.vector3 import Vector3


class TestPerspectiveCamera(unittest.TestCase):

    def test_get_name(self) -> None:
        test = PerspectiveCamera.name
        ref = 'perspective'
        self.assertEqual(test, ref)

    def test_deserialize(self) -> None:
        message = {
            'fovy': 30,
            'aperture_radius': 1,
            'focus_distance': 2
        }
        test = PerspectiveCamera.deserialize(message)
        self.assertAlmostEqual(test.fovy_degrees, 30)
        self.assertEqual(test.aperture_radius, 1)
        self.assertEqual(test.focus_distance, 2)

    def test_fovy_radians(self) -> None:
        test = math.radians(30)
        camera = PerspectiveCamera()
        camera.fovy_radians = test
        self.assertAlmostEqual(camera.fovy_radians, test)
        self.assertAlmostEqual(camera.fovy_degrees, math.degrees(test))

    def test_fovy_degrees(self) -> None:
        test = 30
        camera = PerspectiveCamera()
        camera.fovy_degrees = test
        self.assertAlmostEqual(camera.fovy_degrees, test)
        self.assertAlmostEqual(camera.fovy_radians, math.radians(test))

    def test_serialize(self) -> None:
        camera = PerspectiveCamera(math.radians(30), 1, 2)
        test = camera.serialize()
        self.assertAlmostEqual(test['fovy'], 30)
        self.assertEqual(test['aperture_radius'], 1)
        self.assertEqual(test['focus_distance'], 2)

    def test_get_full_screen_view(self) -> None:
        camera = PerspectiveCamera(fovy=90, degrees=True)
        target = Bounds(-Vector3.one, Vector3.one)
        test = camera.get_full_screen_view(target)
        self.assertAlmostEqual(test.position.x, 0)
        self.assertAlmostEqual(test.position.y, 0)
        self.assertAlmostEqual(test.position.z, 1)
        self.assertEqual(test.target, Vector3.zero)
        self.assertEqual(test.up, Vector3.up)


if __name__ == '__main__':
    unittest.main()
