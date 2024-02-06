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
from typing import cast

import brayns


class TestCameraController(unittest.TestCase):
    def test_camera(self) -> None:
        target = brayns.Bounds(
            min=brayns.Vector3.zero,
            max=brayns.Vector3(4, 1, 3),
        )
        test = brayns.CameraController(
            target,
            aspect_ratio=2,
            translation=brayns.Vector3.one,
            rotation=brayns.CameraRotation.left,
            projection=brayns.OrthographicProjection,
        )
        camera = test.camera
        view = camera.view
        projection = camera.projection
        self.assertAlmostEqual(view.position.x, 1.0)
        self.assertAlmostEqual(view.position.y, 1.5)
        self.assertAlmostEqual(view.position.z, 2.5)
        self.assertEqual(view.target, target.center + brayns.Vector3.one)
        self.assertAlmostEqual(view.up.x, 0)
        self.assertAlmostEqual(view.up.y, 1)
        self.assertAlmostEqual(view.up.z, 0)
        self.assertIsInstance(projection, brayns.OrthographicProjection)
        projection = cast(brayns.OrthographicProjection, projection)
        self.assertAlmostEqual(projection.height, 1.5)

    def test_camera_default(self) -> None:
        target = brayns.Bounds(
            min=-brayns.Vector3.one,
            max=brayns.Vector3.one,
        )
        test = brayns.CameraController(target)
        camera = test.camera
        view = camera.view
        projection = brayns.PerspectiveProjection()
        distance = projection.look_at(target.height) + target.depth / 2
        ref = target.center + distance * brayns.Axis.front
        self.assertAlmostEqual(view.position.x, ref.x)
        self.assertAlmostEqual(view.position.y, ref.y)
        self.assertAlmostEqual(view.position.z, ref.z)
        self.assertEqual(view.target, target.center)
        self.assertAlmostEqual(view.up.x, 0)
        self.assertAlmostEqual(view.up.y, 1)
        self.assertAlmostEqual(view.up.z, 0)
        self.assertEqual(camera.projection, projection)
