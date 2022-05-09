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
from brayns.core.geometry.capsule import Capsule


class TestCapsule(unittest.TestCase):

    def test_name(self) -> None:
        self.assertEqual(Capsule.name, 'capsules')

    def test_properties(self) -> None:
        test = Capsule(
            start_point=Vector3.zero,
            start_radius=1,
            end_point=Vector3.one,
            end_radius=2
        )
        ref = {
            'p0': [0, 0, 0],
            'r0': 1,
            'p1': [1, 1, 1],
            'r1': 2
        }
        self.assertEqual(test.properties, ref)


if __name__ == '__main__':
    unittest.main()
