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


class TestAddClippingGeometries(unittest.TestCase):
    def test_add_clipping_geometry(self) -> None:
        instance = MockInstance(mock_model_message())
        equation = brayns.PlaneEquation(1, 2, 3, 4)
        plane = brayns.ClipPlane(equation)
        test = brayns.add_clipping_geometry(instance, plane)
        self.assertEqual(test, mock_model())
        self.assertEqual(instance.method, brayns.ClipPlane.method)
        self.assertEqual(instance.params, plane.get_properties())

    def test_add_clipping_geometries(self) -> None:
        instance = MockInstance(mock_model_message())
        geometry = brayns.ClippingBox(brayns.Vector3.zero, brayns.Vector3.one)
        test = brayns.add_clipping_geometries(instance, [geometry])
        self.assertEqual(test, mock_model())
        self.assertEqual(instance.method, brayns.ClippingBox.method)
        self.assertEqual(instance.params, [geometry.get_properties()])

    def test_clear_clipping_geometries(self) -> None:
        instance = MockInstance()
        brayns.clear_clipping_geometries(instance)
        self.assertEqual(instance.method, "clear-clip-planes")
        self.assertIsNone(instance.params)

    def test_clip_plane(self) -> None:
        self.assertEqual(brayns.ClipPlane.method, "add-clip-plane")
        equation = brayns.PlaneEquation(1, 2, 3, 4)
        plane = brayns.ClipPlane(equation)
        test = plane.get_properties()
        ref = {"coefficients": [1, 2, 3, 4]}
        self.assertEqual(test, ref)

    def test_clipping_bounded_planes(self) -> None:
        method = brayns.ClippingBoundedPlane.method
        self.assertEqual(method, "add-clipping-bounded-planes")
        equation = brayns.PlaneEquation(1, 2, 3, 4)
        bounds = brayns.Bounds(brayns.Vector3.zero, brayns.Vector3.one)
        plane = brayns.ClippingBoundedPlane(equation, bounds)
        test = plane.get_properties()
        ref = {
            "coefficients": [1, 2, 3, 4],
            "bounds": {
                "min": [0, 0, 0],
                "max": [1, 1, 1],
            },
        }
        self.assertEqual(test, ref)

    def test_clipping_boxes(self) -> None:
        self.assertEqual(brayns.ClippingBox.method, "add-clipping-boxes")
        box = brayns.ClippingBox(brayns.Vector3.zero, brayns.Vector3.one)
        test = box.get_properties()
        ref = {
            "min": [0, 0, 0],
            "max": [1, 1, 1],
        }
        self.assertEqual(test, ref)

    def test_clipping_capsules(self) -> None:
        self.assertEqual(brayns.ClippingCapsule.method, "add-clipping-capsules")
        start = brayns.Vector3.zero
        end = brayns.Vector3.one
        radius = 1
        capsule = brayns.ClippingCapsule(start, radius, end, radius)
        test = capsule.get_properties()
        ref = {
            "p0": [0, 0, 0],
            "r0": 1,
            "p1": [1, 1, 1],
            "r1": 1,
        }
        self.assertEqual(test, ref)

    def test_clipping_planes(self) -> None:
        self.assertEqual(brayns.ClippingPlane.method, "add-clipping-planes")
        equation = brayns.PlaneEquation(1, 2, 3, 4)
        plane = brayns.ClippingPlane(equation)
        test = plane.get_properties()
        ref = {"coefficients": [1, 2, 3, 4]}
        self.assertEqual(test, ref)

    def test_clipping_spheres(self) -> None:
        self.assertEqual(brayns.ClippingSphere.method, "add-clipping-spheres")
        sphere = brayns.ClippingSphere(1)
        test = sphere.get_properties()
        ref = {"center": [0, 0, 0], "radius": 1}
        self.assertEqual(test, ref)
