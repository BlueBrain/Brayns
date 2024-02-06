# Copyright (c) 2015-2024 EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
#
# Responsible Author: nadir.romanguerrero@epfl.ch
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

from __future__ import annotations

from typing import TypeVar

import brayns
from testapi.render import RenderSettings, render_and_validate
from testapi.simple_test_case import SimpleTestCase

T = TypeVar("T", bound=brayns.Geometry)


class TestGeometry(SimpleTestCase):
    def test_bounded_planes(self) -> None:
        geometries = [
            (
                brayns.BoundedPlane(
                    brayns.PlaneEquation(0, 0, 1, 0.5),
                    bounds=brayns.Bounds(
                        min=-brayns.Vector3.one,
                        max=brayns.Vector3.one,
                    ),
                ),
                brayns.Color4.red,
            ),
            (
                brayns.BoundedPlane(
                    brayns.PlaneEquation(0, 0, 1, 0.0),
                    bounds=brayns.Bounds(
                        min=-2 * brayns.Vector3.one,
                        max=brayns.Vector3.one,
                    ),
                ),
                brayns.Color4.blue,
            ),
        ]
        bounds = brayns.Bounds(
            -2 * brayns.Vector3.one,
            brayns.Vector3.one,
        )
        self.run_tests("bounded_planes", geometries, bounds)

    def test_boxes(self) -> None:
        geometries = [
            (
                brayns.Box(
                    min=-brayns.Vector3.one,
                    max=brayns.Vector3(1, 1, 2),
                ),
                brayns.Color4.red,
            ),
            (
                brayns.Box(
                    min=-2 * brayns.Vector3.one,
                    max=brayns.Vector3.one,
                ),
                brayns.Color4.blue,
            ),
        ]
        bounds = brayns.Bounds(
            -2 * brayns.Vector3.one,
            brayns.Vector3(1, 1, 2),
        )
        self.run_tests("boxes", geometries, bounds)

    def test_capsules(self) -> None:
        geometries = [
            (
                brayns.Capsule(
                    start_point=brayns.Vector3.zero,
                    start_radius=0,
                    end_point=brayns.Vector3.one,
                    end_radius=1,
                ),
                brayns.Color4.red,
            ),
            (
                brayns.Capsule(
                    start_point=-brayns.Vector3.one,
                    start_radius=1,
                    end_point=brayns.Vector3.zero,
                    end_radius=0,
                ),
                brayns.Color4.blue,
            ),
        ]
        bounds = brayns.Bounds(
            -2 * brayns.Vector3.one,
            2 * brayns.Vector3.one,
        )
        self.run_tests("capsules", geometries, bounds)

    def test_planes(self) -> None:
        geometries = [
            (
                brayns.Plane(
                    brayns.PlaneEquation(1, 1, 1, 0),
                ),
                brayns.Color4.red,
            ),
            (
                brayns.Plane(
                    brayns.PlaneEquation(0, 0, 1, 0.25),
                ),
                brayns.Color4.blue,
            ),
        ]
        view = brayns.View(
            position=brayns.Axis.z,
            target=brayns.Vector3.zero,
        )
        self.run_tests("planes", geometries, view=view)

    def test_spheres(self) -> None:
        geometries = [
            (
                brayns.Sphere(
                    radius=2,
                ),
                brayns.Color4.red,
            ),
            (
                brayns.Sphere(
                    radius=1,
                    center=2 * brayns.Vector3.one,
                ),
                brayns.Color4.blue,
            ),
        ]
        bounds = brayns.Bounds(
            -2 * brayns.Vector3.one,
            3 * brayns.Vector3.one,
        )
        self.run_tests("spheres", geometries, bounds)

    def run_tests(
        self,
        ref: str,
        geometries: list[tuple[T, brayns.Color4]],
        bounds: brayns.Bounds | None = None,
        view: brayns.View | None = None,
    ) -> None:
        model = brayns.add_geometries(self.instance, geometries)
        retreived = brayns.get_model(self.instance, model.id)
        self.assertEqual(model, retreived)
        if bounds is not None:
            self.assertEqual(model.bounds, bounds)
        self.assertEqual(model.info, {})
        self.assertEqual(model.visible, True)
        self.assertEqual(model.transform, brayns.Transform.identity)
        settings = RenderSettings()
        if view is not None:
            settings.camera = brayns.Camera(view)
            settings.center_camera = False
        render_and_validate(self, ref, settings)
