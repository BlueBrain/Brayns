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

from brayns.core.geometry.box import Box
from brayns.core.vector.vector3 import Vector3


class TestBox(unittest.TestCase):

    def test_method(self) -> None:
        self.assertEqual(Box.method, 'add-boxes')

    def test_serialize(self) -> None:
        box = Box(Vector3.zero, Vector3.one)
        test = box.serialize()
        ref = {
            'min': [0, 0, 0],
            'max': [1, 1, 1],
        }
        self.assertEqual(test, ref)


if __name__ == '__main__':
    unittest.main()