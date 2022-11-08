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


class TestColorModel(unittest.TestCase):

    def test_color_model(self) -> None:
        method = brayns.ColorMethod.single_color(brayns.Color4.red)
        instance = MockInstance()
        brayns.color_model(instance, 1, method)
        self.assertEqual(instance.method, 'color-model')
        self.assertEqual(instance.params, {
            'id': 1,
            'method': method.name,
            'values': {
                key: list(value)
                for key, value in method.colors.items()
            }
        })


if __name__ == '__main__':
    unittest.main()
