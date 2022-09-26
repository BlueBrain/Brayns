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

    def test_vector(self) -> None:
        camera = brayns.Camera()
        self.assertEqual(camera.vector, camera.view.vector)

    def test_direction(self) -> None:
        camera = brayns.Camera()
        self.assertEqual(camera.direction, camera.view.direction)

    def test_distance(self) -> None:
        camera = brayns.Camera()
        self.assertEqual(camera.distance, camera.view.distance)

    def test_look_at(self) -> None:
        camera = brayns.Camera(projection=brayns.OrthographicProjection())
        target = brayns.Bounds(-brayns.Vector3.one, brayns.Vector3.one)
        projection = brayns.OrthographicProjection(target.height)
        view = projection.get_front_view(target)
        camera.look_at(target)
        self.assertEqual(camera.view, view)
        self.assertEqual(camera.projection, projection)

    def test_rotate_around_target(self) -> None:
        euler = brayns.Vector3(1, 2, 3)
        rotation = brayns.Rotation.from_euler(euler, degrees=True)
        test = brayns.Camera()
        ref = brayns.Camera()
        test.rotate_around_target(rotation)
        ref.view.rotate_around_target(rotation)
        self.assertEqual(test, ref)


if __name__ == '__main__':
    unittest.main()
