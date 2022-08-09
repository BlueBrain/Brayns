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
from brayns.plugins.coloring.color_circuit_by_method import color_circuit_by_method
from brayns.plugins.coloring.color_method import ColorMethod
from tests.instance.mock_instance import MockInstance


class TestColorCircuitByMethod(unittest.TestCase):

    def test_color_circuit_by_method(self) -> None:
        instance = MockInstance()
        method = ColorMethod.HEMISPHERE
        colors = {
            'test1': Color4.white,
            'test2': Color4.red
        }
        color_circuit_by_method(instance, 0, method, colors)
        self.assertEqual(instance.method, 'color-circuit-by-method')
        self.assertEqual(instance.params, {
            'model_id': 0,
            'method': method.value,
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
