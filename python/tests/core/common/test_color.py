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

from brayns.core.common.color import Color


class TestColor(unittest.TestCase):

    def test_normalize_hex(self) -> None:
        test = Color.normalize_hex('FF')
        self.assertEqual(test, 1.0)

    def test_from_hex(self) -> None:
        test = Color.from_hex('2ca02c')
        ref = Color.from_int8(44, 160, 44)
        self.assertEqual(test, ref)

    def test_from_int8(self) -> None:
        test = Color.from_int8(33, 160, 44)
        ref = Color(33 / 255, 160 / 255, 44 / 255)
        self.assertEqual(test, ref)

    def test_iter(self) -> None:
        test = list(Color(1, 2, 3, 4))
        ref = [1, 2, 3, 4]
        self.assertEqual(test, ref)

    def test_transparent(self) -> None:
        test = Color(1, 2, 3, 4).transparent
        ref = Color(1, 2, 3, 0)
        self.assertEqual(test, ref)

    def test_opaque(self) -> None:
        test = Color(1, 2, 3, 0).opaque
        ref = Color(1, 2, 3, 1)
        self.assertEqual(test, ref)


if __name__ == '__main__':
    unittest.main()
