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

    def test_get_name(self) -> None:
        test = brayns.PerspectiveCamera.name
        ref = 'perspective'
        self.assertEqual(test, ref)

    def test_deserialize(self) -> None:
        message = {
            'fovy': 30,
            'aperture_radius': 1,
            'focus_distance': 2
        }
        test = brayns.PerspectiveCamera.deserialize(message)
        self.assertAlmostEqual(test.fovy.degrees, 30)
        self.assertEqual(test.aperture_radius, 1)
        self.assertEqual(test.focus_distance, 2)

    def test_serialize(self) -> None:
        camera = brayns.PerspectiveCamera(
            fovy=brayns.Fovy(30, degrees=True),
            aperture_radius=1,
            focus_distance=2
        )
        test = camera.serialize()
        self.assertAlmostEqual(test['fovy'], 30)
        self.assertEqual(test['aperture_radius'], 1)
        self.assertEqual(test['focus_distance'], 2)


if __name__ == '__main__':
    unittest.main()
