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

from brayns.core.common.transform import Transform
from brayns.core.model.model import Model
from tests.core.model.mock_model import MockModel
from tests.instance.mock_instance import MockInstance


class TestModel(unittest.TestCase):

    def setUp(self) -> None:
        self._model = MockModel.model
        self._message = MockModel.serialized_model

    def test_from_instance(self) -> None:
        instance = MockInstance(self._message)
        test = Model.from_instance(instance, 0)
        self.assertEqual(test, self._model)
        self.assertEqual(instance.method, 'get-model')
        self.assertEqual(instance.params, {'id': 0})

    def test_deserialize(self) -> None:
        test = Model.deserialize(self._message)
        self.assertEqual(test, self._model)

    def test_remove(self) -> None:
        instance = MockInstance()
        ids = [1, 2, 3]
        Model.remove(instance, ids)
        self.assertEqual(instance.method, 'remove-model')
        self.assertEqual(instance.params, {'ids': ids})

    def test_update(self) -> None:
        instance = MockInstance()
        Model.update(
            instance,
            id=0,
            visible=True,
            transform=Transform.identity
        )
        self.assertEqual(instance.method, 'update-model')
        self.assertEqual(instance.params, {
            'id': 0,
            'visible': True,
            'transformation': Transform.identity.serialize()
        })


if __name__ == '__main__':
    unittest.main()
