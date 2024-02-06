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


class TestColor3(unittest.TestCase):
    def test_iter(self) -> None:
        test = list(brayns.Color3(1, 2, 3))
        ref = [1, 2, 3]
        self.assertEqual(test, ref)

    def test_parse_numbers(self) -> None:
        test = brayns.parse_hex_color("2ca02c")
        ref = brayns.Color3(44, 160, 44) / 255
        self.assertEqual(test, ref)

    def test_parse_hash(self) -> None:
        test = brayns.parse_hex_color("#2ca02c")
        ref = brayns.Color3(44, 160, 44) / 255
        self.assertEqual(test, ref)

    def test_parse_0x(self) -> None:
        test = brayns.parse_hex_color("0x2ca02c")
        ref = brayns.Color3(44, 160, 44) / 255
        self.assertEqual(test, ref)


class TestColor4(unittest.TestCase):
    def test_from_color3(self) -> None:
        ref = brayns.Color4(1, 2, 3, 4)
        color = brayns.Color3(1, 2, 3)
        test = brayns.Color4.from_color3(color, 4)
        self.assertEqual(test, ref)

    def test_iter(self) -> None:
        test = list(brayns.Color4(1, 2, 3, 4))
        ref = [1, 2, 3, 4]
        self.assertEqual(test, ref)

    def test_transparent(self) -> None:
        test = brayns.Color4(1, 2, 3, 4).transparent
        ref = brayns.Color4(1, 2, 3, 0)
        self.assertEqual(test, ref)

    def test_opaque(self) -> None:
        test = brayns.Color4(1, 2, 3, 0).opaque
        ref = brayns.Color4(1, 2, 3, 1)
        self.assertEqual(test, ref)

    def test_without_alpha(self) -> None:
        color = brayns.Color4(1, 2, 3, 4)
        ref = brayns.Color3(1, 2, 3)
        test = color.without_alpha
        self.assertEqual(test, ref)
