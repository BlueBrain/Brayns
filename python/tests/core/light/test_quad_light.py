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


class TestQuadLight(unittest.TestCase):

    def test_name(self) -> None:
        self.assertEqual(brayns.QuadLight.name, 'quad')

    def test_emission_direction(self) -> None:
        light = brayns.QuadLight(
            edge1=brayns.Axis.up,
            edge2=brayns.Axis.left
        )
        self.assertEqual(light.emission_direction, brayns.Axis.front)

    def test_get_properties(self) -> None:
        test = brayns.QuadLight()
        self.assertEqual(test.get_properties(), {
            'intensity': 1,
            'color': [1, 1, 1],
            'position': [0, 0, 0],
            'edge1': [1, 0, 0],
            'edge2': [0, 1, 0],
        })


if __name__ == '__main__':
    unittest.main()
