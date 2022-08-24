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

from tests.network.mock_instance import MockInstance
from ..model.mock_model import MockModel


class TestMeshLoader(unittest.TestCase):

    def test_name(self) -> None:
        self.assertEqual(brayns.MeshLoader.name, 'mesh')

    def test_properties(self) -> None:
        loader = brayns.MeshLoader()
        self.assertEqual(loader.properties, {})

    def test_load(self) -> None:
        instance = MockInstance([MockModel.message])
        loader = brayns.MeshLoader()
        path = 'path'
        test = loader.load(instance, path)
        ref = [MockModel.model]
        self.assertEqual(test, ref)
        params = loader.serialize(path)
        self.assertEqual(instance.params, params)

    def test_serialize(self) -> None:
        loader = brayns.MeshLoader()
        path = 'path'
        ref = {
            'path': path,
            'loader_name': brayns.MeshLoader.name,
            'loader_properties': loader.properties
        }
        test = loader.serialize(path)
        self.assertEqual(test, ref)


if __name__ == '__main__':
    unittest.main()
