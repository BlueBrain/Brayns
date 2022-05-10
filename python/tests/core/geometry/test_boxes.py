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

from brayns.core.common.box import Box
from brayns.core.common.vector3 import Vector3
from brayns.core.geometry.boxes import Boxes


class TestBoxes(unittest.TestCase):

    def test_name(self) -> None:
        self.assertEqual(Boxes.name, 'boxes')

    def test_serialize_geometry(self) -> None:
        box = Box(Vector3.zero, Vector3.one)
        test = Boxes.serialize_geometry(box)
        ref = box.serialize()
        self.assertEqual(test, ref)


if __name__ == '__main__':
    unittest.main()
