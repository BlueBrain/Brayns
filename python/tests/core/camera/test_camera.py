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

from brayns.core.camera.camera import Camera
from tests.core.camera.mock_camera import MockCamera
from tests.instance.mock_instance import MockInstance


class TestCamera(unittest.TestCase):

    def test_get_main_camera_name(self) -> None:
        name = 'test'
        instance = MockInstance(name)
        test = Camera.get_main_camera_name(instance)
        self.assertEqual(test, name)
        self.assertEqual(instance.method, 'get-camera-type')
        self.assertEqual(instance.params, None)

    def test_from_instance(self) -> None:
        camera = MockCamera()
        instance = MockInstance(camera.serialize())
        test = MockCamera.from_instance(instance)
        self.assertEqual(test, camera)
        self.assertEqual(instance.method, 'get-camera-test')
        self.assertEqual(instance.params, None)

    def test_is_main_camera(self) -> None:
        instance = MockInstance(MockCamera.name)
        test = MockCamera.is_main_camera(instance)
        self.assertTrue(test)
        self.assertEqual(instance.method, 'get-camera-type')
        self.assertEqual(instance.params, None)

    def test_use_as_main_camera(self) -> None:
        instance = MockInstance()
        test = MockCamera('test1', 3)
        test.use_as_main_camera(instance)
        self.assertEqual(instance.method, 'set-camera-test')
        self.assertEqual(instance.params, test.serialize())

    def test_serialize_with_name(self) -> None:
        camera = MockCamera()
        test = camera.serialize_with_name()
        ref = {
            'name': MockCamera.name,
            'params': camera.serialize()
        }
        self.assertEqual(test, ref)


if __name__ == '__main__':
    unittest.main()
