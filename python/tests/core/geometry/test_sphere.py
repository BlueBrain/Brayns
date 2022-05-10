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

from brayns.core.common.sphere import Sphere
from brayns.core.common.vector3 import Vector3
from brayns.core.geometry.spheres import Spheres


class TestSpheres(unittest.TestCase):

    def test_name(self) -> None:
        self.assertEqual(Spheres.name, 'spheres')

    def test_serialize_geometry(self) -> None:
        sphere = Sphere(1, Vector3.one)
        test = Spheres.serialize_geometry(sphere)
        ref = sphere.serialize()
        self.assertEqual(test, ref)


if __name__ == '__main__':
    unittest.main()
