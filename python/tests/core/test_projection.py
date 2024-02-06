# Copyright (c) 2015-2024 EPFL/Blue Brain Project
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

import math
import unittest

import brayns
from tests.mock_instance import MockInstance


class TestProjection(unittest.TestCase):
    def test_get_camera_name(self) -> None:
        ref = "test"
        instance = MockInstance(ref)
        test = brayns.get_camera_name(instance)
        self.assertEqual(test, ref)
        self.assertEqual(instance.method, "get-camera-type")
        self.assertIsNone(instance.params)

    def test_get_camera_projection(self) -> None:
        ref = brayns.OrthographicProjection(3)
        message = ref.get_properties()
        instance = MockInstance(message)
        test = brayns.get_camera_projection(instance, type(ref))
        self.assertEqual(test, ref)
        self.assertEqual(instance.method, "get-camera-orthographic")
        self.assertIsNone(instance.params)

    def test_set_camera_projection(self) -> None:
        instance = MockInstance()
        test = brayns.OrthographicProjection(3)
        brayns.set_camera_projection(instance, test)
        self.assertEqual(instance.method, "set-camera-orthographic")
        self.assertEqual(instance.params, test.get_properties())


class TestFovy(unittest.TestCase):
    def test_eq(self) -> None:
        self.assertEqual(brayns.Fovy(30), brayns.Fovy(30.0))
        self.assertNotEqual(brayns.Fovy(31), brayns.Fovy(30.0))
        self.assertNotEqual(brayns.Fovy(31), None)

    def test_radians(self) -> None:
        angle = math.radians(30)
        fovy = brayns.Fovy(angle)
        self.assertAlmostEqual(fovy.radians, angle)
        self.assertAlmostEqual(fovy.degrees, math.degrees(angle))

    def test_degrees(self) -> None:
        angle = 30
        fovy = brayns.Fovy(angle, degrees=True)
        self.assertAlmostEqual(fovy.radians, math.radians(angle))
        self.assertAlmostEqual(fovy.degrees, angle)

    def test_get_distance(self) -> None:
        fovy = brayns.Fovy(90, degrees=True)
        test = fovy.get_distance(2)
        self.assertAlmostEqual(test, 1)


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


class TestOrthographicProjection(unittest.TestCase):
    def test_name(self) -> None:
        test = brayns.OrthographicProjection.name
        ref = "orthographic"
        self.assertEqual(test, ref)

    def test_look_at(self) -> None:
        test = brayns.OrthographicProjection()
        height = 1
        distance = test.look_at(height)
        self.assertEqual(test.height, height)
        self.assertEqual(distance, 0)

    def test_get_properties(self) -> None:
        projection = brayns.OrthographicProjection(3)
        test = projection.get_properties()
        self.assertEqual(test, {"height": 3})

    def test_update_properties(self) -> None:
        test = brayns.OrthographicProjection()
        test.update_properties({"height": 3})
        self.assertEqual(test.height, 3)
