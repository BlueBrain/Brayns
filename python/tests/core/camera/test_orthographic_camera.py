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


class TestOrthographicCamera(unittest.TestCase):

    def test_get_front_view(self) -> None:
        target = brayns.Bounds(-brayns.Vector3.one, brayns.Vector3.one)
        test = brayns.OrthographicCamera.get_front_view(target)
        self.assertEqual(test.position, 2 * brayns.Vector3.forward)
        self.assertEqual(test.target, brayns.Vector3.zero)
        self.assertEqual(test.up, brayns.Vector3.up)

    def test_name(self) -> None:
        test = brayns.OrthographicCamera.name
        ref = 'orthographic'
        self.assertEqual(test, ref)

    def test_deserialize(self) -> None:
        message = {
            'height': 3
        }
        test = brayns.OrthographicCamera.deserialize(message)
        self.assertEqual(test.height, 3)

    def test_serialize(self) -> None:
        camera = brayns.OrthographicCamera(3)
        test = camera.serialize()
        self.assertEqual(test['height'], 3)

    def test_serialize_with_name(self) -> None:
        camera = brayns.OrthographicCamera()
        test = camera.serialize_with_name()
        ref = {
            'name': brayns.OrthographicCamera.name,
            'params': camera.serialize(),
        }
        self.assertEqual(test, ref)


if __name__ == '__main__':
    unittest.main()
