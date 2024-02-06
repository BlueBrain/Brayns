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

import unittest
from typing import Any

import brayns
from tests.mock_instance import MockInstance
from tests.mock_messages import (
    mock_bounds,
    mock_bounds_message,
    mock_model,
    mock_model_message,
)


class TestAddClippingGeometries(unittest.TestCase):
    def test_clear_clipping_geometries(self) -> None:
        instance = MockInstance()
        brayns.clear_clipping_geometries(instance)
        self.assertEqual(instance.method, "clear-clipping-geometries")
        self.assertIsNone(instance.params)

    def test_bounded_planes(self) -> None:
        geometry = brayns.BoundedPlane(
            equation=brayns.PlaneEquation(1, 2, 3, 4), bounds=mock_bounds()
        )
        message = {
            "coefficients": [1, 2, 3, 4],
            "bounds": mock_bounds_message(),
        }
        self.run_geometry_tests("add-bounded-planes", geometry, message)
        self.run_clipping_tests("add-clipping-bounded-planes", geometry, message)

    def test_boxes(self) -> None:
        geometry = brayns.Box(brayns.Vector3.zero, brayns.Vector3.one)
        message = {
            "min": [0, 0, 0],
            "max": [1, 1, 1],
        }
        self.run_geometry_tests("add-boxes", geometry, message)
        self.run_clipping_tests("add-clipping-boxes", geometry, message)

    def test_capsules(self) -> None:
        geometry = brayns.Capsule(brayns.Vector3.zero, 1, brayns.Vector3.one, 1)
        message = {
            "p0": [0, 0, 0],
            "r0": 1,
            "p1": [1, 1, 1],
            "r1": 1,
        }
        self.run_geometry_tests("add-capsules", geometry, message)
        self.run_clipping_tests("add-clipping-capsules", geometry, message)

    def test_planes(self) -> None:
        geometry = brayns.Plane(brayns.PlaneEquation(1, 2, 3, 4))
        message = {"coefficients": [1, 2, 3, 4]}
        self.run_geometry_tests("add-planes", geometry, message)
        self.run_clipping_tests("add-clipping-planes", geometry, message)

    def test_spheres(self) -> None:
        geometry = brayns.Sphere(1, brayns.Vector3(1, 2, 3))
        message = {"center": [1, 2, 3], "radius": 1}
        self.run_geometry_tests("add-spheres", geometry, message)
        self.run_clipping_tests("add-clipping-spheres", geometry, message)

    def test_clipping_normal_inversion(self) -> None:
        geometry = brayns.Sphere(1, brayns.Vector3(1, 2, 3))
        message = {"center": [1, 2, 3], "radius": 1}
        self.run_clipping_tests("add-clipping-spheres", geometry, message)
        self.run_clipping_tests("add-clipping-spheres", geometry, message, True)

    def run_geometry_tests(
        self,
        method: str,
        geometry: brayns.Geometry,
        message: dict[str, Any],
    ) -> None:
        instance = MockInstance(mock_model_message())
        color = brayns.Color4.white
        model = brayns.add_geometries(instance, [(geometry, color)])
        self.assertEqual(model, mock_model())
        self.assertEqual(instance.method, method)
        self.assertEqual(instance.params, [{"geometry": message, "color": list(color)}])

    def run_clipping_tests(
        self,
        method: str,
        geometry: brayns.Geometry,
        message: dict[str, Any],
        invert_normals: bool = False,
    ) -> None:
        instance = MockInstance(mock_model_message())
        model = brayns.add_clipping_geometries(instance, [geometry], invert_normals)
        self.assertEqual(model, mock_model())
        self.assertEqual(instance.method, method)
        self.assertEqual(
            instance.params,
            {
                "primitives": [message],
                "invert_normals": invert_normals,
            },
        )
