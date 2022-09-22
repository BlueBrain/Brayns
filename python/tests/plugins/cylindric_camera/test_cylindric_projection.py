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


class TestCylindricProjection(unittest.TestCase):

    def test_get_name(self) -> None:
        test = brayns.CylindricProjection.name
        ref = 'cylindric'
        self.assertEqual(test, ref)

    def test_get_front_view(self) -> None:
        target = brayns.Bounds(-brayns.Vector3.one, brayns.Vector3.one)
        projection = brayns.CylindricProjection()
        test = projection.get_front_view(target)
        ref = projection.fovy.get_front_view(target)
        self.assertEqual(test, ref)

    def test_set_target(self) -> None:
        target = brayns.Bounds(-brayns.Vector3.one, brayns.Vector3.one)
        test = brayns.CylindricProjection()
        test.set_target(target)
        ref = brayns.CylindricProjection()
        self.assertEqual(test, ref)

    def test_get_properties(self) -> None:
        projection = brayns.CylindricProjection(
            fovy=brayns.Fovy(30, degrees=True),
        )
        test = projection.get_properties()
        self.assertAlmostEqual(test['fovy'], 30)

    def test_update_properties(self) -> None:
        test = brayns.CylindricProjection()
        test.update_properties({'fovy': 30})
        self.assertAlmostEqual(test.fovy.degrees, 30)


if __name__ == '__main__':
    unittest.main()
