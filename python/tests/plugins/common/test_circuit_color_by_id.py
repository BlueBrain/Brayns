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

from brayns.core.color.color4 import Color4
from brayns.plugins.common.cell_id import CellId
from brayns.plugins.common.circuit_color_by_id import CircuitColorById
from tests.plugins.common.mock_circuit_instance import MockCircuitInstance


class TestCircuitColorById(unittest.TestCase):

    def test_apply(self) -> None:
        instance = MockCircuitInstance()
        color = CircuitColorById({
            CellId.from_integer(3): Color4.white,
            CellId.from_range(4, 6): Color4.red
        })
        not_colored = color.apply(instance, 0)
        self.assertEqual(instance.method, 'color-circuit-by-id')
        self.assertEqual(instance.params, {
            'model_id': 0,
            'color_info': [
                {
                    'variable': '3',
                    'color': [1, 1, 1, 1]
                },
                {
                    'variable': '4-6',
                    'color': [1, 0, 0, 1]
                }
            ]
        })
        self.assertEqual(not_colored, instance.not_colored)


if __name__ == '__main__':
    unittest.main()
