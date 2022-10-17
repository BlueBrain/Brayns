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

import brayns
from testapi.simple_test_case import SimpleTestCase


class TestAddPlanes(SimpleTestCase):

    def test_add_planes(self) -> None:
        test = brayns.add_geometries(self.instance, [
            brayns.Plane(
                brayns.PlaneEquation(1, 2, 3)
            ).with_color(brayns.Color4.red),
            brayns.Plane(brayns.PlaneEquation(4, 5, 6, 7))
        ])
        ref = brayns.get_model(self.instance, test.id)
        self.assertEqual(test, ref)
        self.assertEqual(test.info, {})
        self.assertEqual(test.visible, True)
        self.assertEqual(test.transform, brayns.Transform.identity)
