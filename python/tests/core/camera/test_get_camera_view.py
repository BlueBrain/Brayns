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
from tests.mock_instance import MockInstance


class TestGetCameraView(unittest.TestCase):

    def test_get_camera_view(self) -> None:
        message = {
            'position': [1, 2, 3],
            'target': [4, 5, 6],
            'up': [7, 8, 9],
        }
        instance = MockInstance(message)
        view = brayns.get_camera_view(instance)
        self.assertEqual(instance.method, 'get-camera-look-at')
        self.assertEqual(instance.params, None)
        self.assertEqual(view.position, brayns.Vector3(1, 2, 3))
        self.assertEqual(view.target, brayns.Vector3(4, 5, 6))
        self.assertEqual(view.up, brayns.Vector3(7, 8, 9))


if __name__ == '__main__':
    unittest.main()
