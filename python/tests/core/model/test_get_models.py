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

from brayns.core.model.get_models import get_models
from tests.core.model.mock_model import MockModel
from tests.instance.mock_instance import MockInstance


class TestGetModels(unittest.TestCase):

    def setUp(self) -> None:
        self._scene = {
            'models': [
                MockModel.message,
                MockModel.message,
            ]
        }
        self._models = [
            MockModel.model,
            MockModel.model,
        ]

    def test_get_models(self) -> None:
        instance = MockInstance(self._scene)
        test = get_models(instance)
        self.assertEqual(test, self._models)
        self.assertEqual(instance.method, 'get-scene')
        self.assertEqual(instance.params, None)


if __name__ == '__main__':
    unittest.main()