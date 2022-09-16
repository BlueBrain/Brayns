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

import brayns
from tests.mock_instance import MockInstance


class TestSetColorRamp(unittest.TestCase):

    def test_set_color_ramp(self) -> None:
        instance = MockInstance()
        brayns.set_color_ramp(instance, 0, brayns.ColorRamp(
            value_range=brayns.ValueRange(1, 2),
            colors=[
                brayns.Color4.red,
                brayns.Color4.blue
            ],
        ))
        self.assertEqual(instance.method, 'set-model-transfer-function')
        self.assertEqual(instance.params, {
            'id': 0,
            'transfer_function': {
                'range': [1, 2],
                'colors': [
                    [1, 0, 0, 1],
                    [0, 0, 1, 1],
                ],
            }
        })


if __name__ == '__main__':
    unittest.main()
