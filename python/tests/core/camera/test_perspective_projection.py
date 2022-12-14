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


class TestPerspectiveProjection(unittest.TestCase):
    def test_get_name(self) -> None:
        test = brayns.PerspectiveProjection.name
        ref = "perspective"
        self.assertEqual(test, ref)

    def test_look_at(self) -> None:
        projection = brayns.PerspectiveProjection()
        height = 1
        distance = projection.look_at(height)
        ref = projection.fovy.get_distance(height)
        self.assertEqual(projection, brayns.PerspectiveProjection())
        self.assertEqual(distance, ref)

    def test_get_properties(self) -> None:
        projection = brayns.PerspectiveProjection(
            fovy=brayns.Fovy(30, degrees=True),
            aperture_radius=1,
            focus_distance=2,
        )
        test = projection.get_properties()
        self.assertEqual(len(test), 3)
        self.assertAlmostEqual(test["fovy"], 30)
        self.assertEqual(test["aperture_radius"], 1)
        self.assertEqual(test["focus_distance"], 2)

    def test_update_properties(self) -> None:
        test = brayns.PerspectiveProjection()
        test.update_properties(
            {
                "fovy": 30,
                "aperture_radius": 1,
                "focus_distance": 2,
            }
        )
        self.assertAlmostEqual(test.fovy.degrees, 30)
        self.assertEqual(test.aperture_radius, 1)
        self.assertEqual(test.focus_distance, 2)
