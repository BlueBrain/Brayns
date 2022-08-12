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
from tests.instance.mock_instance import MockInstance


class TestGetColorMethodValues(unittest.TestCase):

    def test_get_color_method_values(self) -> None:
        ref = ['test1', 'test2']
        instance = MockInstance({'variables': ref})
        method = brayns.ColorMethod.ETYPE
        test = brayns.get_color_method_values(instance, 0, method)
        self.assertEqual(instance.method, 'get-circuit-color-method-variables')
        self.assertEqual(instance.params, {
            'model_id': 0,
            'method': method.value
        })
        self.assertEqual(test, ref)


if __name__ == '__main__':
    unittest.main()
