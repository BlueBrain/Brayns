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

import math
import unittest

import brayns


class TestRotation(unittest.TestCase):

    def test_from_quaternion(self) -> None:
        quaternion = brayns.Quaternion(1, 2, 3, 4)
        test = brayns.Rotation.from_quaternion(quaternion)
        self.assertEqual(test.quaternion, quaternion.normalized)

    def test_from_euler(self) -> None:
        euler = brayns.Vector3(34, -22, -80)
        test = brayns.Rotation.from_euler(euler, degrees=True)
        quaternion = test.quaternion
        self.assertAlmostEqual(quaternion.norm, 1)
        self.assertAlmostEqual(quaternion.x, 0.10256431)
        self.assertAlmostEqual(quaternion.y, -0.32426137)
        self.assertAlmostEqual(quaternion.z, -0.56067163)
        self.assertAlmostEqual(quaternion.w, 0.75497182)

    def test_from_axis_angle(self) -> None:
        axis = brayns.Vector3(1, 2, 3)
        angle = 30
        test = brayns.Rotation.from_axis_angle(axis, angle, degrees=True)
        quaternion = test.quaternion
        self.assertAlmostEqual(quaternion.norm, 1)
        self.assertAlmostEqual(quaternion.x, 0.0691723)
        self.assertAlmostEqual(quaternion.y, 0.1383446)
        self.assertAlmostEqual(quaternion.z, 0.2075169)
        self.assertAlmostEqual(quaternion.w, 0.96592583)

    def test_deserialize(self) -> None:
        test = brayns.Rotation.deserialize([0, 0, 0, 1])
        quaternion = brayns.Quaternion(0, 0, 0, 1)
        ref = brayns.Rotation.from_quaternion(quaternion)
        self.assertEqual(test, ref)

    def test_identity(self) -> None:
        quaternion = brayns.Quaternion.identity
        self.assertEqual(brayns.Rotation.identity, brayns.Rotation(quaternion))

    def test_quaternion(self) -> None:
        quaternion = brayns.Quaternion(1, 2, 3, 4)
        rotation = brayns.Rotation.from_quaternion(quaternion)
        self.assertEqual(rotation.quaternion, quaternion.normalized)

    def test_euler_radians(self) -> None:
        ref = brayns.Vector3(34, -22, -80)
        rotation = brayns.Rotation.from_euler(ref, degrees=True)
        test = rotation.euler_radians
        self.assertAlmostEqual(test.x, math.radians(ref.x))
        self.assertAlmostEqual(test.y, math.radians(ref.y))
        self.assertAlmostEqual(test.z, math.radians(ref.z))

    def test_euler_degrees(self) -> None:
        ref = brayns.Vector3(34, -22, -80)
        rotation = brayns.Rotation.from_euler(ref, degrees=True)
        test = rotation.euler_degrees
        self.assertAlmostEqual(test.x, ref.x)
        self.assertAlmostEqual(test.y, ref.y)
        self.assertAlmostEqual(test.z, ref.z)

    def test_axis(self) -> None:
        ref = brayns.Vector3.up
        rotation = brayns.Rotation.from_axis_angle(
            brayns.Vector3.up, 30, degrees=True)
        test = rotation.axis.normalized
        self.assertAlmostEqual(test.x, ref.x)
        self.assertAlmostEqual(test.y, ref.y)
        self.assertAlmostEqual(test.z, ref.z)

    def test_angle_radians(self) -> None:
        rotation = brayns.Rotation.from_axis_angle(
            brayns.Vector3.up, 30, degrees=True)
        self.assertAlmostEqual(rotation.angle_radians, math.radians(30))

    def test_angle_degrees(self) -> None:
        rotation = brayns.Rotation.from_axis_angle(
            brayns.Vector3.up, 30, degrees=True)
        self.assertAlmostEqual(rotation.angle_degrees, 30)

    def test_inverse(self) -> None:
        quaternion = brayns.Quaternion(1, 2, 3, 4).normalized
        rotation = brayns.Rotation.from_quaternion(quaternion)
        self.assertEqual(rotation.inverse.quaternion, quaternion.inverse)

    def test_serialize(self) -> None:
        quaternion = brayns.Quaternion(1, 2, 3, 4)
        rotation = brayns.Rotation.from_quaternion(quaternion)
        test = rotation.serialize()
        self.assertEqual(test, list(quaternion.normalized))

    def test_combine(self) -> None:
        r1 = brayns.Rotation.from_axis_angle(
            brayns.Vector3.up, 30, degrees=True)
        r2 = brayns.Rotation.from_axis_angle(
            brayns.Vector3.up, 30, degrees=True)
        ref = r1.quaternion * r2.quaternion
        test = r1.combine(r2).quaternion
        self.assertEqual(test, ref)

    def test_apply(self) -> None:
        rotation = brayns.Rotation.from_euler(
            brayns.Vector3(22, 35, 68), degrees=True)
        value = brayns.Vector3(1, 2, 3)
        test = rotation.apply(value)
        self.assertAlmostEqual(test.x, 0.3881471)
        self.assertAlmostEqual(test.y, 2.91087149)
        self.assertAlmostEqual(test.z, 2.31865673)

    def test_apply_center(self) -> None:
        rotation = brayns.Rotation.from_euler(
            brayns.Vector3(22, 35, 68), degrees=True)
        value = brayns.Vector3(1, 2, 3)
        center = brayns.Vector3(4, 5, 6)
        test = rotation.apply(value, center)
        self.assertAlmostEqual(test.x, 3.77731325)
        self.assertAlmostEqual(test.y, 0.02357039)
        self.assertAlmostEqual(test.z, 4.52163639)


if __name__ == '__main__':
    unittest.main()
