# Copyright (c) 2015-2022 EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
#
# Responsible Author: nadir.romanguerrero@epfl.ch
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


class TestAddBoundedPlanes(SimpleTestCase):

    def test_add_bounded_planes(self) -> None:
        test = brayns.add_geometries(self.instance, [
            brayns.BoundedPlane(
                brayns.PlaneEquation(0, 0, 1, 0),
                bounds=brayns.Bounds(
                    min=-brayns.Vector3.one,
                    max=brayns.Vector3.one
                )
            ).with_color(brayns.Color4.red),
            brayns.BoundedPlane(
                brayns.PlaneEquation(0, 0, 1, 0.5),
                bounds=brayns.Bounds(
                    min=-2 * brayns.Vector3.one,
                    max=brayns.Vector3.one
                )
            ).with_color(brayns.Color4.blue),
        ])
        ref = brayns.get_model(self.instance, test.id)
        self.assertEqual(test, ref)
        self.assertEqual(test.bounds, brayns.Bounds(
            -2 * brayns.Vector3.one,
            brayns.Vector3.one
        ))
        self.assertEqual(test.info, {})
        self.assertEqual(test.visible, True)
        self.assertEqual(test.transform, brayns.Transform.identity)
