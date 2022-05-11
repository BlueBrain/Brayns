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

from tests.core.model.mock_model import MockModel
from tests.core.model.mock_model_loader import MockModelLoader
from tests.instance.mock_instance import MockInstance


class TestModelLoader(unittest.TestCase):

    def test_load(self) -> None:
        reply = [MockModel.serialized_model]
        ref = [MockModel.model]
        instance = MockInstance(reply)
        loader = MockModelLoader(1, 'test')
        path = 'path/test.model'
        test = loader.load(instance, path)
        self.assertEqual(test, ref)
        self.assertEqual(instance.method, 'add-model')
        self.assertEqual(instance.params, {
            'path': path,
            'loader': MockModelLoader.name,
            'loader_properties': loader.properties
        })


if __name__ == '__main__':
    unittest.main()
