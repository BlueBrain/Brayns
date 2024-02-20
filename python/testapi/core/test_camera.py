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
from testapi.render import RenderSettings, render_and_validate
from testapi.simple_test_case import SimpleTestCase


def run_projection_tests(
    context: SimpleTestCase, projection: brayns.Projection
) -> None:
    brayns.set_camera_projection(context.instance, projection)
    name = brayns.get_camera_name(context.instance)
    context.assertEqual(name, projection.name)
    test = brayns.get_camera_projection(context.instance, type(projection))
    context.assertEqual(test, projection)
    brayns.clear_models(context.instance)
    spheres = [
        (brayns.Sphere(1), brayns.Color4.red),
        (brayns.Sphere(1, 3 * brayns.Vector3.one), brayns.Color4.blue),
    ]
    brayns.add_geometries(context.instance, spheres)
    camera = brayns.Camera(projection=projection)
    settings = RenderSettings(camera=camera)
    render_and_validate(context, f"{name}_projection", settings)


class TestCamera(SimpleTestCase):
    def test_get_camera_view(self) -> None:
        test = brayns.get_camera_view(self.instance)
        ref = brayns.View(
            position=brayns.Vector3.zero,
            target=brayns.Axis.front,
        )
        self.assertEqual(test, ref)

    def test_set_camera_view(self) -> None:
        view = brayns.View(
            position=brayns.Vector3(1, 2, 3),
            target=brayns.Vector3(4, 5, 6),
            up=brayns.Vector3(7, 8, 9),
        )
        brayns.set_camera_view(self.instance, view)
        ref = brayns.get_camera_view(self.instance)
        self.assertEqual(view, ref)

    def test_get_camera(self) -> None:
        test = brayns.get_camera(self.instance, brayns.PerspectiveProjection)
        self.assertIsInstance(test.projection, brayns.PerspectiveProjection)

    def test_set_camera(self) -> None:
        ref = brayns.Camera()
        brayns.set_camera(self.instance, ref)
        test = brayns.get_camera(self.instance, type(ref.projection))
        self.assertEqual(test, ref)

    def test_orthographic_projection(self) -> None:
        projection = brayns.OrthographicProjection(height=10)
        run_projection_tests(self, projection)

    def test_perspective_projection(self) -> None:
        projection = brayns.PerspectiveProjection(
            fovy=brayns.Fovy(30, degrees=True),
            aperture_radius=1,
            focus_distance=2,
        )
        run_projection_tests(self, projection)

    def test_getset_near_clip(self) -> None:
        distance = brayns.get_camera_near_clip(self.instance)
        self.assertEqual(distance, 1e-6)
        brayns.set_camera_near_clip(self.instance, 2)
        distance = brayns.get_camera_near_clip(self.instance)
        self.assertEqual(distance, 2)
        camera = brayns.get_camera(self.instance, brayns.PerspectiveProjection)
        self.assertEqual(camera.near_clipping_distance, 2)

    def test_render_near_clip(self) -> None:
        close = brayns.Sphere(0.5, brayns.Vector3(0, 0, -2))
        far = brayns.Sphere(0.5, brayns.Vector3(0, 0, -4))
        brayns.add_geometries(
            self.instance,
            [(close, brayns.Color4.red), (far, brayns.Color4.blue)],
        )
        view = brayns.View.front
        camera = brayns.Camera(view)
        settings = RenderSettings(camera=camera, center_camera=False)
        render_and_validate(self, "clip_two", settings)
        camera.near_clipping_distance = 3
        render_and_validate(self, "clip_one", settings)
