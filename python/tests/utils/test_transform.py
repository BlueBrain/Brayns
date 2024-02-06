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
from brayns.utils import deserialize_transform, serialize_transform
from tests.mock_messages import mock_transform, mock_transform_message


class TestTransform(unittest.TestCase):
    def test_rotate(self) -> None:
        rotation = brayns.euler(0.25, 1.5, 2.5)
        center = brayns.Vector3(1, 2, 3)
        transform = brayns.Transform.rotate(rotation, center)
        value = brayns.Vector3(4, 5, 6)
        test = transform.apply(value)
        ref = rotation.apply(value, center)
        self.assertAlmostEqual(test.x, ref.x)
        self.assertAlmostEqual(test.y, ref.y)
        self.assertAlmostEqual(test.z, ref.z)

    def test_identity(self) -> None:
        test = brayns.Transform.identity
        self.assertEqual(test.translation, brayns.Vector3.zero)
        self.assertEqual(test.rotation, brayns.Rotation.identity)
        self.assertEqual(test.scale, brayns.Vector3.one)

    def test_apply(self) -> None:
        transform = brayns.Transform(
            translation=brayns.Vector3.one,
            rotation=brayns.euler(0, 0, 90, degrees=True),
            scale=2 * brayns.Vector3.one,
        )
        value = brayns.Vector3.one
        test = transform.apply(value)
        self.assertAlmostEqual(test.x, -1)
        self.assertAlmostEqual(test.y, 3)
        self.assertAlmostEqual(test.z, 3)

    def test_deserialize_transform(self) -> None:
        test = deserialize_transform(mock_transform_message())
        self.assertEqual(test, mock_transform())

    def test_serialize_transform(self) -> None:
        test = serialize_transform(mock_transform())
        self.assertEqual(test, mock_transform_message())
