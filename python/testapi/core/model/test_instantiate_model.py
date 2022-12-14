# Copyright (c) 2015-2022 EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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


class TestInstantiateModel(SimpleTestCase):
    def test_instantiate(self) -> None:
        sphere = brayns.Sphere(20).with_color(brayns.Color4.red)
        model = brayns.add_geometries(self.instance, [sphere])

        ref_position = brayns.Vector3(0, -30, 0)
        ref = brayns.Sphere(5, ref_position).with_color(brayns.Color4.blue)
        brayns.add_geometries(self.instance, [ref])

        instance_transform = brayns.Transform(brayns.Vector3(50, 0, 0))
        brayns.instantiate_model(self.instance, model.id, instance_transform)

        ref = self.folder / "instantiate.png"
        self.quick_validation(ref)
