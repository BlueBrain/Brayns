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


class TestGetCamera(unittest.TestCase):

    def test_get_camera(self) -> None:
        message = {
            'fovy': 45,
            'aperture_radius': 0,
            'focus_distance': 1,
        }
        instance = MockInstance(message)
        test = brayns.get_camera(instance, brayns.PerspectiveCamera)
        self.assertEqual(test.fovy.degrees, 45)
        self.assertEqual(test.aperture_radius, 0)
        self.assertEqual(test.focus_distance, 1)
        self.assertEqual(instance.method, 'get-camera-perspective')
        self.assertEqual(instance.params, None)


if __name__ == '__main__':
    unittest.main()
