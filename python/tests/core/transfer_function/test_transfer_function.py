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
from brayns.core.transfer_function.transfer_function import TransferFunction
from brayns.core.transfer_function.value_range import ValueRange
from tests.instance.mock_instance import MockInstance


class TestTransferFunction(unittest.TestCase):

    def setUp(self) -> None:
        self._function = TransferFunction(
            value_range=ValueRange(0, 1),
            colors=[
                Color4.red,
                Color4.blue
            ]
        )
        self._message = {
            'range': [0, 1],
            'colors': [
                [1, 0, 0, 1],
                [0, 0, 1, 1]
            ]
        }

    def test_from_model(self) -> None:
        instance = MockInstance(self._message)
        test = TransferFunction.from_model(instance, 0)
        self.assertEqual(test, self._function)
        self.assertEqual(instance.method, 'get-model-transfer-function')
        self.assertEqual(instance.params, {'id': 0})

    def test_deserialize(self) -> None:
        test = TransferFunction.deserialize(self._message)
        self.assertEqual(test, self._function)

    def test_apply(self) -> None:
        instance = MockInstance()
        self._function.apply(instance, 0)
        self.assertEqual(instance.method, 'set-model-transfer-function')
        self.assertEqual(instance.params, {
            'id': 0,
            'transfer_function': self._message
        })

    def test_serialize(self) -> None:
        test = self._function.serialize()
        self.assertEqual(test, self._message)


if __name__ == '__main__':
    unittest.main()
