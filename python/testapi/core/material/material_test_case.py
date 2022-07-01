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


class MaterialTestCase(SimpleTestCase):

    def run_tests(self, material: brayns.Material) -> None:
        boxes = brayns.Boxes([
            (
                brayns.Box(-brayns.Vector3.one, brayns.Vector3.one),
                brayns.Color4.red
            )
        ])
        model = boxes.add(self.instance)
        id = model.id
        material.apply(self.instance, id)
        name = brayns.Material.get_material_name(self.instance, id)
        self.assertEqual(name, material.name)
        applied = material.is_applied(self.instance, id)
        self.assertTrue(applied)
        test = material.from_model(self.instance, id)
        self.assertEqual(test, material)
