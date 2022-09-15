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


class TestPerspectiveCamera(unittest.TestCase):

    def setUp(self) -> None:
        self.target = brayns.Bounds(
            min=-brayns.Vector3.one,
            max=brayns.Vector3.one,
        )

    def test_get_name(self) -> None:
        test = brayns.PerspectiveCamera.name
        ref = 'perspective'
        self.assertEqual(test, ref)

    def test_get_front_view(self) -> None:
        camera = brayns.PerspectiveCamera()
        test = camera.get_front_view(self.target)
        ref = camera.fovy.get_front_view(self.target)
        self.assertEqual(test, ref)

    def test_from_target(self) -> None:
        test = brayns.PerspectiveCamera()
        ref = brayns.PerspectiveCamera()
        test.set_target(self.target)
        self.assertEqual(test, ref)

    def test_get_properties(self) -> None:
        camera = brayns.PerspectiveCamera(
            fovy=brayns.Fovy(30, degrees=True),
            aperture_radius=1,
            focus_distance=2,
        )
        test = camera.get_properties()
        self.assertAlmostEqual(test['fovy'], 30)
        self.assertEqual(test['aperture_radius'], 1)
        self.assertEqual(test['focus_distance'], 2)

    def test_update_properties(self) -> None:
        message = {
            'fovy': 30,
            'aperture_radius': 1,
            'focus_distance': 2,
        }
        test = brayns.PerspectiveCamera()
        test.update_properties(message)
        self.assertAlmostEqual(test.fovy.degrees, 30)
        self.assertEqual(test.aperture_radius, 1)
        self.assertEqual(test.focus_distance, 2)


if __name__ == '__main__':
    unittest.main()
