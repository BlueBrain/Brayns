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

from brayns.core.common.vector3 import Vector3
from brayns.core.light.quad_light import QuadLight


class TestQuadLight(unittest.TestCase):

    def test_name(self) -> None:
        self.assertEqual(QuadLight.name, 'quad')

    def test_emission_direction(self) -> None:
        light = QuadLight(
            bottom_left=Vector3.one,
            edge1=Vector3.up,
            edge2=Vector3.left
        )
        self.assertEqual(light.emission_direction, Vector3.forward)

    def test_serialize(self) -> None:
        light = QuadLight()
        ref = {
            'color': [1, 1, 1],
            'intensity': 1,
            'visible': True,
            'bottom_left_corner': [0, 0, 0],
            'vertical_displacement': [0, 1, 0],
            'horizontal_displacement': [1, 0, 0]
        }
        test = light.serialize()
        self.assertEqual(test, ref)


if __name__ == '__main__':
    unittest.main()
