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

from brayns.core.common.color4 import Color4
from brayns.plugins.common.circuit_color_by_method import CircuitColorByMethod
from brayns.plugins.common.color_method import ColorMethod
from tests.plugins.common.mock_circuit_instance import MockCircuitInstance


class TestCircuitColorByMethod(unittest.TestCase):

    def test_get_available_methods(self) -> None:
        instance = MockCircuitInstance()
        test = CircuitColorByMethod.get_available_methods(instance, 0)
        ref = [ColorMethod(method) for method in instance.methods]
        self.assertEqual(test, ref)
        self.assertEqual(instance.method, 'get-circuit-color-methods')
        self.assertEqual(instance.params, {
            'model_id': 0
        })

    def test_get_available_values(self) -> None:
        instance = MockCircuitInstance()
        values = CircuitColorByMethod.get_available_values(
            instance=instance,
            model_id=0,
            method=ColorMethod.ETYPE
        )
        self.assertEqual(instance.method, 'get-circuit-color-method-variables')
        self.assertEqual(instance.params, {
            'model_id': 0,
            'method': ColorMethod.ETYPE.value
        })
        self.assertEqual(values, instance.values)

    def test_apply(self) -> None:
        instance = MockCircuitInstance()
        color = CircuitColorByMethod(ColorMethod.ETYPE, {
            'test1': Color4.white,
            'test2': Color4.red
        })
        color.apply(instance, 0)
        self.assertEqual(instance.method, 'color-circuit-by-method')
        self.assertEqual(instance.params, {
            'model_id': 0,
            'method': ColorMethod.ETYPE.value,
            'color_info': [
                {
                    'variable': 'test1',
                    'color': [1, 1, 1, 1]
                },
                {
                    'variable': 'test2',
                    'color': [1, 0, 0, 1]
                }
            ]
        })


if __name__ == '__main__':
    unittest.main()
