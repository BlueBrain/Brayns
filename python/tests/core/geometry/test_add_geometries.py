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
from brayns.core.geometry.add_geometries import add_geometries
from brayns.core.geometry.plane import Plane
from tests.core.model.mock_model import MockModel
from tests.instance.mock_instance import MockInstance


class TestAddGeometries(unittest.TestCase):

    def setUp(self) -> None:
        self._geometries = [
            Plane(1, 2, 3, 4),
            Plane(5, 6, 7, 8).with_color(Color4.red),
        ]
        self._message = [
            {
                'geometry': {
                    'value': [1, 2, 3, 4]
                },
                'color': [1, 1, 1, 1]
            },
            {
                'geometry': {
                    'value': [5, 6, 7, 8]
                },
                'color': [1, 0, 0, 1]
            }
        ]

    def test_add_geometries(self) -> None:
        ref = MockModel.model
        reply = MockModel.message
        instance = MockInstance(reply)
        model = add_geometries(instance, self._geometries)
        self.assertEqual(model, ref)
        self.assertEqual(instance.method, 'add-planes')
        self.assertEqual(instance.params, self._message)


if __name__ == '__main__':
    unittest.main()
