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
from tests.mock_instance import MockInstance
from tests.mock_messages import mock_view, mock_view_message


class TestCameraView(unittest.TestCase):
    def test_get_camera_view(self) -> None:
        instance = MockInstance(mock_view_message())
        test = brayns.get_camera_view(instance)
        self.assertEqual(test, mock_view())
        self.assertEqual(instance.method, "get-camera-view")
        self.assertIsNone(instance.params)

    def test_set_camera_view(self) -> None:
        instance = MockInstance()
        brayns.set_camera_view(instance, mock_view())
        self.assertEqual(instance.method, "set-camera-view")
        self.assertEqual(instance.params, mock_view_message())


class TestView(unittest.TestCase):
    def test_front(self) -> None:
        test = brayns.View.front
        self.assertEqual(test.direction, brayns.Axis.back)
        self.assertEqual(test.up, brayns.Axis.up)

    def test_vector(self) -> None:
        test = brayns.View(
            position=brayns.Vector3.zero,
            target=brayns.Vector3.one,
        )
        self.assertEqual(test.vector, brayns.Vector3.one)

    def test_direction(self) -> None:
        test = brayns.View(
            position=brayns.Vector3.zero,
            target=brayns.Axis.front,
        )
        self.assertEqual(test.direction, brayns.Axis.front)

    def test_right(self) -> None:
        test = brayns.View(
            position=brayns.Vector3.zero,
            target=brayns.Axis.back,
        )
        self.assertEqual(test.right, brayns.Axis.right)

    def test_real_up(self) -> None:
        test = brayns.View(
            position=brayns.Vector3.zero,
            target=brayns.Axis.back,
        )
        self.assertEqual(test.real_up, brayns.Axis.up)

    def test_distance(self) -> None:
        test = brayns.View.front
        self.assertEqual(test.distance, 1)
        test.distance = 3
        self.assertEqual(test.distance, 3)

    def test_orientation(self) -> None:
        test = brayns.View.front
        ref = test.get_orientation(brayns.View.front)
        self.assertEqual(test.orientation, ref)

    def test_get_orientation(self) -> None:
        test = brayns.View(
            position=brayns.Vector3(1, 2, 3),
            target=brayns.Vector3.zero,
            up=brayns.Vector3(4, 5, 6),
        )
        ref = brayns.View.front
        orientation = test.get_orientation(ref)
        direction = orientation.apply(ref.direction)
        self.assertAlmostEqual(direction.x, test.direction.x)
        self.assertAlmostEqual(direction.y, test.direction.y)
        self.assertAlmostEqual(direction.z, test.direction.z)
        up = orientation.apply(ref.real_up)
        self.assertAlmostEqual(up.x, test.real_up.x)
        self.assertAlmostEqual(up.y, test.real_up.y)
        self.assertAlmostEqual(up.z, test.real_up.z)

    def test_translate(self) -> None:
        translation = brayns.Vector3(1, 2, 3)
        ref = brayns.View(
            position=brayns.Vector3.zero,
            target=brayns.Vector3.one,
        )
        test = ref.translate(translation)
        self.assertEqual(test.position, ref.position + translation)
        self.assertEqual(test.target, ref.target + translation)
        self.assertEqual(test.up, ref.up)

    def test_rotate_around_target(self) -> None:
        rotation = brayns.euler(0, 180, 0, degrees=True)
        test = brayns.View(
            position=brayns.Vector3.zero,
            target=brayns.Axis.front,
            up=brayns.Axis.right,
        )
        test = test.rotate_around_target(rotation)
        ref = 2 * brayns.Axis.front
        self.assertAlmostEqual(test.position.x, ref.x)
        self.assertAlmostEqual(test.position.y, ref.y)
        self.assertAlmostEqual(test.position.z, ref.z)
        self.assertEqual(test.target, brayns.Axis.front)
        ref = brayns.Axis.left
        self.assertAlmostEqual(test.up.x, ref.x)
        self.assertAlmostEqual(test.up.y, ref.y)
        self.assertAlmostEqual(test.up.z, ref.z)
