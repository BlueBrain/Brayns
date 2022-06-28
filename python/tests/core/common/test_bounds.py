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

from brayns.core.common.bounds import Bounds
from brayns.core.common.vector3 import Vector3
from tests.core.common.mock_bounds import MockBounds


class TestBounds(unittest.TestCase):

    def setUp(self) -> None:
        self._box = Bounds(Vector3(1, 2, 3), Vector3(4, 5, 6))

    def test_empty(self) -> None:
        test = Bounds.empty
        self.assertEqual(test.min, Vector3.zero)
        self.assertEqual(test.max, Vector3.zero)

    def test_deserialize(self) -> None:
        test = Bounds.deserialize(MockBounds.serialized_bounds)
        self.assertEqual(test, MockBounds.bounds)

    def test_center(self) -> None:
        self.assertEqual(self._box.center, Vector3(2.5, 3.5, 4.5))

    def test_size(self) -> None:
        self.assertEqual(self._box.size, Vector3(3, 3, 3))

    def test_width(self) -> None:
        self.assertEqual(self._box.width, 3)

    def test_height(self) -> None:
        self.assertEqual(self._box.height, 3)

    def test_depth(self) -> None:
        self.assertEqual(self._box.depth, 3)


if __name__ == '__main__':
    unittest.main()
