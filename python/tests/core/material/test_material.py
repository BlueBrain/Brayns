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

from brayns.core.common.color import Color
from brayns.core.material.material import Material
from tests.core.material.mock_material import MockMaterial
from tests.instance.mock_instance import MockInstance


class TestMaterial(unittest.TestCase):

    def test_get_material_name(self) -> None:
        name = 'test'
        instance = MockInstance(name)
        test = Material.get_material_name(instance, 0)
        self.assertEqual(test, name)
        self.assertEqual(instance.method, 'get-material-type')
        self.assertEqual(instance.params, {'id': 0})

    def test_from_model(self) -> None:
        material = MockMaterial()
        reply = material.serialize()
        instance = MockInstance(reply)
        test = MockMaterial.from_model(instance, 0)
        self.assertEqual(test, material)
        self.assertEqual(instance.method, 'get-material-test')
        self.assertEqual(instance.params, {'id': 0})

    def test_is_applied(self) -> None:
        instance = MockInstance(MockMaterial.name)
        self.assertTrue(MockMaterial.is_applied(instance, 0))

    def test_apply(self) -> None:
        instance = MockInstance()
        material = MockMaterial(Color.blue, test1='test', test2=3)
        material.apply(instance, 0)
        self.assertEqual(instance.method, 'set-material-test')
        self.assertEqual(instance.params, {
            'model_id': 0,
            'material': material.serialize()
        })


if __name__ == '__main__':
    unittest.main()
