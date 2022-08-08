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


class TestAddCapsules(SimpleTestCase):

    def test_add_capsules(self) -> None:
        capsules = [
            brayns.Capsule(
                start_point=brayns.Vector3.zero,
                start_radius=0,
                end_point=brayns.Vector3.one,
                end_radius=1,
            ).with_color(brayns.Color4.red),
            brayns.Capsule(
                start_point=-brayns.Vector3.one,
                start_radius=1,
                end_point=brayns.Vector3.one,
                end_radius=0,
            ).with_color(brayns.Color4.blue),
        ]
        model = brayns.add_geometries(self.instance, capsules)
        self.assertEqual(model.id, 0)
        self.assertEqual(model.bounds, brayns.Bounds(
            -2 * brayns.Vector3.one,
            2 * brayns.Vector3.one
        ))
        self.assertEqual(model.metadata, {})
        self.assertEqual(model.visible, True)
        self.assertEqual(model.transform, brayns.Transform.identity)
