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


class TestCylindricCamera(unittest.TestCase):

    def test_get_name(self) -> None:
        test = brayns.CylindricCamera.name
        ref = 'cylindric'
        self.assertEqual(test, ref)

    def test_from_target(self) -> None:
        target = brayns.Bounds(-brayns.Vector3.one, brayns.Vector3.one)
        test = brayns.CylindricCamera.from_target(target)
        ref = brayns.CylindricCamera()
        self.assertEqual(test, ref)

    def test_deserialize(self) -> None:
        message = {
            'fovy': 30
        }
        test = brayns.CylindricCamera.deserialize(message)
        self.assertAlmostEqual(test.fovy.degrees, 30)

    def test_get_front_view(self) -> None:
        target = brayns.Bounds(-brayns.Vector3.one, brayns.Vector3.one)
        camera = brayns.CylindricCamera()
        test = camera.get_front_view(target)
        ref = camera.fovy.get_front_view(target)
        self.assertEqual(test, ref)

    def test_serialize(self) -> None:
        camera = brayns.CylindricCamera(
            fovy=brayns.Fovy(30, degrees=True)
        )
        test = camera.serialize()
        self.assertAlmostEqual(test['fovy'], 30)


if __name__ == '__main__':
    unittest.main()
