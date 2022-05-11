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
from tests.core.geometry.mock_geometries import MockGeometries
from tests.core.model.mock_model import MockModel
from tests.instance.mock_instance import MockInstance


class TestGeometries(unittest.TestCase):

    def setUp(self) -> None:
        self._geometries = MockGeometries([
            (1, Color.white),
            (2, Color.red)
        ])
        self._message = [
            {
                'geometry': 1,
                'color': [1, 1, 1, 1]
            },
            {
                'geometry': 2,
                'color': [1, 0, 0, 1]
            }
        ]

    def test_add(self) -> None:
        ref = MockModel.model
        reply = MockModel.serialized_model
        instance = MockInstance(reply)
        model = self._geometries.add(instance)
        self.assertEqual(model, ref)
        self.assertEqual(instance.method, 'add-tests')
        self.assertEqual(instance.params, self._message)

    def test_serialize(self) -> None:
        test = self._geometries.serialize()
        self.assertEqual(test, self._message)


if __name__ == '__main__':
    unittest.main()
