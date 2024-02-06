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

import brayns


class TestCamera(unittest.TestCase):
    def test_name(self) -> None:
        camera = brayns.Camera()
        self.assertEqual(camera.name, brayns.PerspectiveProjection.name)

    def test_position(self) -> None:
        camera = brayns.Camera()
        self.assertEqual(camera.position, camera.view.position)
        camera.position = brayns.Vector3.one
        self.assertEqual(camera.position, brayns.Vector3.one)

    def test_target(self) -> None:
        camera = brayns.Camera()
        self.assertEqual(camera.target, camera.view.target)
        camera.target = brayns.Vector3.one
        self.assertEqual(camera.target, brayns.Vector3.one)

    def test_up(self) -> None:
        camera = brayns.Camera()
        self.assertEqual(camera.up, camera.view.up)
        camera.up = brayns.Vector3.one
        self.assertEqual(camera.up, brayns.Vector3.one)

    def test_right(self) -> None:
        camera = brayns.Camera()
        self.assertEqual(camera.right, camera.view.right)

    def test_real_up(self) -> None:
        camera = brayns.Camera()
        self.assertEqual(camera.real_up, camera.view.real_up)

    def test_vector(self) -> None:
        camera = brayns.Camera()
        self.assertEqual(camera.vector, camera.view.vector)

    def test_direction(self) -> None:
        camera = brayns.Camera()
        self.assertEqual(camera.direction, camera.view.direction)

    def test_distance(self) -> None:
        camera = brayns.Camera()
        self.assertEqual(camera.distance, camera.view.distance)
        camera.distance = 3
        self.assertEqual(camera.distance, 3)

    def test_orientation(self) -> None:
        camera = brayns.Camera()
        self.assertEqual(camera.orientation, camera.view.orientation)

    def test_translate(self) -> None:
        translation = brayns.Vector3(1, 2, 3)
        ref = brayns.Camera()
        view = ref.view.translate(translation)
        test = ref.translate(translation)
        self.assertEqual(test.view, view)
        self.assertEqual(test.projection, ref.projection)
        self.assertIsNot(test.projection, ref.projection)

    def test_rotate_around_target(self) -> None:
        rotation = brayns.euler(1, 2, 3, degrees=True)
        ref = brayns.Camera()
        view = ref.view.rotate_around_target(rotation)
        test = ref.rotate_around_target(rotation)
        self.assertEqual(test.view, view)
        self.assertEqual(test.projection, ref.projection)
        self.assertIsNot(test.projection, ref.projection)
