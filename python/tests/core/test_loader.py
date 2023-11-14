# Copyright (c) 2015-2023 EPFL/Blue Brain Project
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

import dataclasses
import unittest
from typing import Any

import brayns
from tests.mock_instance import MockInstance
from tests.mock_messages import (
    mock_model,
    mock_model_message,
    mock_schema,
    mock_schema_message,
)


class TestLoader(unittest.TestCase):
    def test_get_loaders(self) -> None:
        instance = MockInstance(
            [
                self.mock_message(),
                self.mock_message() | {"name": "test2"},
            ]
        )
        test = brayns.get_loaders(instance)
        self.assertEqual(
            test,
            [
                self.mock_loader(),
                dataclasses.replace(self.mock_loader(), name="test2"),
            ],
        )
        self.assertIsNone(instance.params)

    def test_mesh_loader(self) -> None:
        self.assertEqual(brayns.MeshLoader.name, "mesh")
        loader = brayns.MeshLoader()
        self.assertEqual(loader.get_properties(), {})

    def test_mhd_volume_loader(self) -> None:
        self.assertEqual(brayns.MhdVolumeLoader.name, "mhd-volume")
        loader = brayns.MhdVolumeLoader()
        self.assertEqual(loader.get_properties(), {})

    def test_raw_volume_loader(self) -> None:
        self.assertEqual(brayns.RawVolumeLoader.name, "raw-volume")
        dimensions = brayns.Vector3.one
        spacing = brayns.Vector3.one
        data_type = brayns.VolumeDataType.DOUBLE
        loader = brayns.RawVolumeLoader(dimensions, spacing, data_type)
        self.assertEqual(
            loader.get_properties(),
            {
                "dimensions": list(dimensions),
                "spacing": list(spacing),
                "data_type": data_type.value,
            },
        )

    def mock_loader(self) -> brayns.LoaderInfo:
        return brayns.LoaderInfo(
            name="test",
            extensions=["test1", "test2"],
            schema=mock_schema(),
            binary=True,
            plugin="Plugin",
        )

    def mock_message(self) -> dict[str, Any]:
        return {
            "name": "test",
            "extensions": ["test1", "test2"],
            "input_parameters_schema": mock_schema_message(),
            "binary": True,
            "plugin": "Plugin",
        }


class TestLoadAndUpload(unittest.TestCase):
    def test_load_models(self) -> None:
        instance = MockInstance([mock_model_message(), mock_model_message()])
        loader = brayns.MeshLoader()
        path = "path"
        test = loader.load_models(instance, path)
        self.assertEqual(test, [mock_model(), mock_model()])
        self.assertEqual(instance.method, "add-model")
        self.assertEqual(
            instance.params,
            {
                "path": path,
                "loader_name": brayns.MeshLoader.name,
                "loader_properties": loader.get_properties(),
            },
        )

    def test_load_models_task(self) -> None:
        instance = MockInstance([mock_model_message(), mock_model_message()])
        loader = brayns.MeshLoader()
        path = "path"
        task = loader.load_models_task(instance, path)
        models = task.wait_for_result()
        self.assertEqual(models, [mock_model(), mock_model()])
        self.assertEqual(instance.method, "add-model")
        self.assertEqual(
            instance.params,
            {
                "path": path,
                "loader_name": brayns.MeshLoader.name,
                "loader_properties": loader.get_properties(),
            },
        )

    def test_upload_models(self) -> None:
        instance = MockInstance([mock_model_message(), mock_model_message()])
        loader = brayns.MeshLoader()
        format = loader.PLY
        data = b"123"
        test = loader.upload_models(instance, format, data)
        self.assertEqual(test, [mock_model(), mock_model()])
        self.assertEqual(instance.method, "upload-model")
        self.assertEqual(
            instance.params,
            {
                "type": format,
                "loader_name": loader.name,
                "loader_properties": loader.get_properties(),
            },
        )
        self.assertEqual(instance.binary, data)

    def test_upload_models_task(self) -> None:
        instance = MockInstance([mock_model_message(), mock_model_message()])
        loader = brayns.MeshLoader()
        format = loader.PLY
        data = b"123"
        task = loader.upload_models_task(instance, format, data)
        models = task.wait_for_result()
        self.assertEqual(models, [mock_model(), mock_model()])
        self.assertEqual(instance.method, "upload-model")
        self.assertEqual(
            instance.params,
            {
                "type": format,
                "loader_name": loader.name,
                "loader_properties": loader.get_properties(),
            },
        )
        self.assertEqual(instance.binary, data)
