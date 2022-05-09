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
from tests.core.geometry.mock_geometry import MockGeometry
from tests.core.model.mock_model import MockModel
from tests.instance.mock_instance import MockInstance


class TestGeometry(unittest.TestCase):

    def test_add(self) -> None:
        ref = MockModel.model
        reply = MockModel.serialized_model
        instance = MockInstance(reply)
        tests = [
            MockGeometry(0, 'test0'),
            MockGeometry(1, 'test1').with_color(Color.red)
        ]
        model = MockGeometry.add(instance, tests)
        self.assertEqual(model, ref)
        self.assertEqual(instance.method, 'add-tests')
        self.assertEqual(instance.params, [
            test.serialize()
            for test in tests
        ])

    def test_serialize(self) -> None:
        geometry = MockGeometry(0, 'test')
        geometry.color = Color.red
        test = geometry.serialize()
        ref = {
            'geometry': {
                'test1': 0,
                'test2': 'test'
            },
            'color': [1, 0, 0, 1]
        }
        self.assertEqual(test, ref)

    def test_with_color(self) -> None:
        geometry = MockGeometry(0, 'test')
        test = geometry.with_color(Color.red)
        self.assertEqual(geometry.color, Color.white)
        self.assertEqual(test.color, Color.red)


if __name__ == '__main__':
    unittest.main()
