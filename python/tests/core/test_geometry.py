# Copyright (c) 2015-2023 EPFL/Blue Brain Project
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
from tests.mock_messages import mock_model, mock_model_message


class TestGeometries(unittest.TestCase):
    def test_add_geometries(self) -> None:
        instance = MockInstance(mock_model_message())
        model = brayns.add_geometries(
            instance,
            [
                brayns.Plane(brayns.PlaneEquation(1, 2, 3, 4)),
                brayns.Plane(brayns.PlaneEquation(5, 6, 7, 8)).with_color(
                    brayns.Color4.red
                ),
            ],
        )
        self.assertEqual(model, mock_model())
        self.assertEqual(instance.method, "add-planes")
        self.assertEqual(
            instance.params,
            [
                {
                    "geometry": {
                        "coefficients": [1, 2, 3, 4],
                    },
                    "color": [1, 1, 1, 1],
                },
                {
                    "geometry": {
                        "coefficients": [5, 6, 7, 8],
                    },
                    "color": [1, 0, 0, 1],
                },
            ],
        )

    def test_bounded_plane(self) -> None:
        self.assertEqual(brayns.BoundedPlane.method, "add-bounded-planes")
        bounded_plane = brayns.BoundedPlane(
            brayns.PlaneEquation(0, 0, 1, 0),
            bounds=brayns.Bounds(
                min=brayns.Vector3.zero,
                max=brayns.Vector3.one,
            ),
        )
        test = bounded_plane.get_additional_properties()
        self.assertEqual(
            test,
            {
                "coefficients": [0, 0, 1, 0],
                "bounds": {
                    "min": [0, 0, 0],
                    "max": [1, 1, 1],
                },
            },
        )

    def test_box(self) -> None:
        self.assertEqual(brayns.Box.method, "add-boxes")
        box = brayns.Box(
            min=brayns.Vector3.zero,
            max=brayns.Vector3.one,
        )
        test = box.get_additional_properties()
        self.assertEqual(
            test,
            {
                "min": [0, 0, 0],
                "max": [1, 1, 1],
            },
        )

    def test_capsule(self) -> None:
        self.assertEqual(brayns.Capsule.method, "add-capsules")
        capsule = brayns.Capsule(
            start_point=brayns.Vector3.zero,
            start_radius=0,
            end_point=brayns.Vector3.one,
            end_radius=1,
        )
        test = capsule.get_additional_properties()
        self.assertEqual(
            test,
            {
                "p0": [0, 0, 0],
                "r0": 0,
                "p1": [1, 1, 1],
                "r1": 1,
            },
        )

    def test_plane(self) -> None:
        self.assertEqual(brayns.Plane.method, "add-planes")
        equation = brayns.PlaneEquation(1, 2, 3, 4)
        plane = brayns.Plane(equation)
        test = plane.get_additional_properties()
        self.assertEqual(test, {"coefficients": [1, 2, 3, 4]})

    def test_sphere(self) -> None:
        self.assertEqual(brayns.Sphere.method, "add-spheres")
        sphere = brayns.Sphere(1, brayns.Vector3.one)
        test = sphere.get_additional_properties()
        self.assertEqual(
            test,
            {
                "center": [1, 1, 1],
                "radius": 1,
            },
        )
