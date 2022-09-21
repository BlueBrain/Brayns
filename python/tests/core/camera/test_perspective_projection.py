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
from tests.mock_bounds import MockBounds


class TestPerspectiveProjection(unittest.TestCase):

    def test_get_name(self) -> None:
        test = brayns.PerspectiveProjection.name
        ref = 'perspective'
        self.assertEqual(test, ref)

    def test_get_front_view(self) -> None:
        target = MockBounds.bounds
        projection = brayns.PerspectiveProjection()
        test = projection.get_front_view(target)
        ref = projection.fovy.get_front_view(target)
        self.assertEqual(test, ref)

    def test_set_target(self) -> None:
        target = MockBounds.bounds
        test = brayns.PerspectiveProjection()
        ref = brayns.PerspectiveProjection()
        test.set_target(target)
        self.assertEqual(test, ref)

    def test_get_properties(self) -> None:
        projection = brayns.PerspectiveProjection(
            fovy=brayns.Fovy(30, degrees=True),
            aperture_radius=1,
            focus_distance=2,
        )
        test = projection.get_properties()
        self.assertEqual(len(test), 3)
        self.assertAlmostEqual(test['fovy'], 30)
        self.assertEqual(test['aperture_radius'], 1)
        self.assertEqual(test['focus_distance'], 2)

    def test_update_properties(self) -> None:
        test = brayns.PerspectiveProjection()
        test.update_properties({
            'fovy': 30,
            'aperture_radius': 1,
            'focus_distance': 2,
        })
        self.assertAlmostEqual(test.fovy.degrees, 30)
        self.assertEqual(test.aperture_radius, 1)
        self.assertEqual(test.focus_distance, 2)


if __name__ == '__main__':
    unittest.main()
