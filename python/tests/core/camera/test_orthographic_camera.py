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

from brayns.core.camera.orthographic_camera import OrthographicCamera
from brayns.core.common.bounds import Bounds
from brayns.core.common.vector3 import Vector3


class TestPerspectiveCamera(unittest.TestCase):

    def test_from_target(self) -> None:
        target = Bounds(-Vector3.one, Vector3.one)
        test = OrthographicCamera.from_target(target)
        self.assertEqual(test.height, 2)

    def test_name(self) -> None:
        test = OrthographicCamera.name
        ref = 'orthographic'
        self.assertEqual(test, ref)

    def test_deserialize(self) -> None:
        message = {
            'height': 3
        }
        test = OrthographicCamera.deserialize(message)
        self.assertEqual(test.height, 3)

    def test_serialize(self) -> None:
        camera = OrthographicCamera(3)
        test = camera.serialize()
        self.assertAlmostEqual(test['height'], 3)

    def test_get_full_screen_view(self) -> None:
        target = Bounds(-Vector3.one, Vector3.one)
        camera = OrthographicCamera.from_target(target)
        test = camera.get_full_screen_view(target)
        self.assertEqual(test.position, 2 * Vector3.forward)
        self.assertEqual(test.target, Vector3.zero)
        self.assertEqual(test.up, Vector3.up)


if __name__ == '__main__':
    unittest.main()
