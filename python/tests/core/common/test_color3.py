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

from brayns.core.color.color3 import Color3


class TestColor3(unittest.TestCase):

    def test_normalize_hex(self) -> None:
        test = Color3.normalize_hex('FF')
        self.assertEqual(test, 1.0)

    def test_from_hex(self) -> None:
        test = Color3.from_hex('2ca02c')
        ref = Color3(44, 160, 44) / 255
        self.assertEqual(test, ref)
        test = Color3.from_hex('0x2ca02c')
        ref = Color3(44, 160, 44) / 255
        self.assertEqual(test, ref)
        test = Color3.from_hex('#2ca02c')
        ref = Color3(44, 160, 44) / 255
        self.assertEqual(test, ref)

    def test_iter(self) -> None:
        test = list(Color3(1, 2, 3))
        ref = [1, 2, 3]
        self.assertEqual(test, ref)


if __name__ == '__main__':
    unittest.main()
