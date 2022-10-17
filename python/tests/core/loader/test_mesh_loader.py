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
from tests.mock_model import MockModel


class TestMeshLoader(unittest.TestCase):

    def test_name(self) -> None:
        self.assertEqual(brayns.MeshLoader.name, 'mesh')

    def test_get_properties(self) -> None:
        loader = brayns.MeshLoader()
        self.assertEqual(loader.get_properties(), {})

    def test_load_models(self) -> None:
        instance = MockInstance([MockModel.message, MockModel.message])
        loader = brayns.MeshLoader()
        path = 'path'
        test = loader.load_models(instance, path)
        self.assertEqual(test, [MockModel.model, MockModel.model])
        self.assertEqual(instance.method, 'add-model')
        self.assertEqual(instance.params, {
            'path': path,
            'loader_name': brayns.MeshLoader.name,
            'loader_properties': loader.get_properties(),
        })

    def test_load_models_task(self) -> None:
        instance = MockInstance([MockModel.message, MockModel.message])
        loader = brayns.MeshLoader()
        path = 'path'
        task = loader.load_models_task(instance, path)
        models = task.wait_for_result()
        self.assertEqual(models, [MockModel.model, MockModel.model])
        self.assertEqual(instance.method, 'add-model')
        self.assertEqual(instance.params, {
            'path': path,
            'loader_name': brayns.MeshLoader.name,
            'loader_properties': loader.get_properties(),
        })

    def test_upload_models(self) -> None:
        instance = MockInstance([MockModel.message, MockModel.message])
        loader = brayns.MeshLoader()
        format = loader.PLY
        data = b'123'
        test = loader.upload_models(instance, format, data)
        self.assertEqual(test, [MockModel.model, MockModel.model])
        self.assertEqual(instance.method, 'upload-model')
        self.assertEqual(instance.params, {
            'type': format,
            'loader_name': loader.name,
            'loader_properties': loader.get_properties(),
        })
        self.assertEqual(instance.binary, data)

    def test_upload_models_task(self) -> None:
        instance = MockInstance([MockModel.message, MockModel.message])
        loader = brayns.MeshLoader()
        format = loader.PLY
        data = b'123'
        task = loader.upload_models_task(instance, format, data)
        models = task.wait_for_result()
        self.assertEqual(models, [MockModel.model, MockModel.model])
        self.assertEqual(instance.method, 'upload-model')
        self.assertEqual(instance.params, {
            'type': format,
            'loader_name': loader.name,
            'loader_properties': loader.get_properties(),
        })
        self.assertEqual(instance.binary, data)


if __name__ == '__main__':
    unittest.main()
