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


class TestColorMethod(unittest.TestCase):

    @classmethod
    @property
    def colors(cls) -> dict[str, brayns.Color4]:
        return {
            '1': brayns.Color4.red,
            '2': brayns.Color4.blue,
        }

    def test_single_color(self) -> None:
        color = brayns.Color4.red
        test = brayns.ColorMethod.single_color(color)
        self.assertEqual(test.name, 'solid')
        self.assertEqual(test.colors, {'color': color})

    def test_geometry(self) -> None:
        test = brayns.ColorMethod.geometry(self.colors)
        self.assertEqual(test.name, 'primitive')
        self.assertEqual(test.colors, self.colors)

    def test_triangle(self) -> None:
        test = brayns.ColorMethod.triangle(self.colors)
        self.assertEqual(test.name, 'triangle')
        self.assertEqual(test.colors, self.colors)

    def test_vertex(self) -> None:
        test = brayns.ColorMethod.vertex(self.colors)
        self.assertEqual(test.name, 'vertex')
        self.assertEqual(test.colors, self.colors)


if __name__ == '__main__':
    unittest.main()
