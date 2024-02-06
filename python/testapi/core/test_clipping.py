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


import brayns
from testapi.loading import add_clip_plane, add_light, add_sphere
from testapi.render import RenderSettings, render_and_validate
from testapi.simple_test_case import SimpleTestCase


class TestClipping(SimpleTestCase):
    def test_clear_clipping_geometries(self) -> None:
        models = [
            add_sphere(self),
            add_light(self),
            add_clip_plane(self),
            add_clip_plane(self),
        ]
        brayns.clear_clipping_geometries(self.instance)
        brayns.get_model(self.instance, models[0].id)
        brayns.get_model(self.instance, models[1].id)
        with self.assertRaises(brayns.JsonRpcError):
            brayns.get_model(self.instance, models[2].id)
        with self.assertRaises(brayns.JsonRpcError):
            brayns.get_model(self.instance, models[3].id)

    def test_clipping_bounded_plane(self) -> None:
        equation = brayns.PlaneEquation(0, 0, 1, 0)
        bound_min = brayns.Vector3(-11, 0, 0)
        bound_max = brayns.Vector3(0, 11, 0.1)
        bounds = brayns.Bounds(bound_min, bound_max)
        plane = brayns.BoundedPlane(equation, bounds)
        self.run_tests(plane, "bounded_plane")

    def test_clipping_box(self) -> None:
        box_min = brayns.Vector3(0, -5, 0)
        box_max = brayns.Vector3(10, 10, 10)
        box = brayns.Box(box_min, box_max)
        self.run_tests(box, "box")

    def test_clipping_capsule(self) -> None:
        rotation = brayns.euler(0, 45, 0, degrees=True)
        start = rotation.apply(brayns.Vector3(0, 0, 9))
        end = rotation.apply(brayns.Vector3(0, 10, 9))
        capsule = brayns.Capsule(start, 7, end, 3)
        self.run_tests(capsule, "capsule")

    def test_clipping_plane(self) -> None:
        rotation = brayns.euler(0, 45, 0, degrees=True)
        vector = rotation.apply(brayns.Vector3(0, 0, -1))
        equation = brayns.PlaneEquation(*vector)
        plane = brayns.Plane(equation)
        self.run_tests(plane, "plane")

    def test_clipping_sphere(self) -> None:
        rotation = brayns.euler(0, 45, 0, degrees=True)
        center = rotation.apply(brayns.Vector3(0, 0, 9))
        sphere = brayns.Sphere(7, center)
        self.run_tests(sphere, "sphere")

    def test_clipping_inverted_normals(self) -> None:
        rotation = brayns.euler(0, 45, 0, degrees=True)
        center = rotation.apply(brayns.Vector3(0, 0, 9))
        sphere = brayns.Sphere(7, center)
        self.run_tests(sphere, "inverted_normals", True)

    def run_tests(
        self,
        geometry: brayns.Geometry,
        ref: str,
        invert_normals: bool = False,
    ) -> None:
        brayns.add_clipping_geometries(self.instance, [geometry], invert_normals)
        sphere = brayns.Sphere(10)
        color = brayns.Color4.red
        brayns.add_geometries(self.instance, [(sphere, color)])
        renderer = brayns.InteractiveRenderer(enable_shadows=False)
        settings = RenderSettings(renderer=renderer)
        render_and_validate(self, f"clipping_{ref}", settings)
