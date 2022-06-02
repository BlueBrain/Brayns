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

from brayns.core.camera.camera_view import CameraView
from brayns.core.common.vector3 import Vector3
from tests.instance.mock_instance import MockInstance


class TestCameraView(unittest.TestCase):

    def setUp(self) -> None:
        self._view = CameraView(
            position=Vector3(1, 2, 3),
            target=Vector3(4, 5, 6),
            up=Vector3(7, 8, 9)
        )
        self._message = {
            'position': [1, 2, 3],
            'target': [4, 5, 6],
            'up': [7, 8, 9]
        }

    def test_from_instance(self) -> None:
        instance = MockInstance(self._message)
        test = CameraView.from_instance(instance)
        self.assertEqual(test, self._view)
        self.assertEqual(instance.method, 'get-camera-look-at')
        self.assertEqual(instance.params, None)

    def test_deserialize(self) -> None:
        test = CameraView.deserialize(self._message)
        self.assertEqual(test, self._view)

    def test_direction(self) -> None:
        test = CameraView(
            position=Vector3.zero,
            target=Vector3.one
        )
        self.assertEqual(test.direction, Vector3.one)

    def test_use_for_main_camera(self) -> None:
        instance = MockInstance()
        self._view.use_for_main_camera(instance)
        self.assertEqual(instance.method, 'set-camera-look-at')
        self.assertEqual(instance.params, self._message)

    def test_serialize(self) -> None:
        test = self._view.serialize()
        self.assertEqual(test, self._message)


if __name__ == '__main__':
    unittest.main()
