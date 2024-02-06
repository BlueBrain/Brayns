# Copyright (c) 2015-2024 EPFL/Blue Brain Project
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
    mock_bounds,
    mock_bounds_message,
    mock_model,
    mock_model_message,
    mock_transform,
    mock_transform_message,
)


class TestModel(unittest.TestCase):
    def test_get_model(self) -> None:
        instance = MockInstance(mock_model_message())
        test = brayns.get_model(instance, 0)
        self.assertEqual(test, mock_model())
        self.assertEqual(instance.method, "get-model")
        self.assertEqual(instance.params, {"id": 0})

    def test_remove_models(self) -> None:
        instance = MockInstance()
        ids = [1, 2, 3]
        brayns.remove_models(instance, ids)
        self.assertEqual(instance.method, "remove-model")
        self.assertEqual(instance.params, {"ids": ids})

    def test_clear_models(self) -> None:
        instance = MockInstance()
        brayns.clear_models(instance)
        self.assertEqual(instance.method, "clear-models")
        self.assertIsNone(instance.params)

    def test_clear_renderables(self) -> None:
        instance = MockInstance()
        brayns.clear_renderables(instance)
        self.assertEqual(instance.method, "clear-renderables")
        self.assertIsNone(instance.params)

    def test_get_scene(self) -> None:
        instance = MockInstance(self.mock_scene_message())
        test = brayns.get_scene(instance)
        self.assertEqual(test, self.mock_scene())
        self.assertEqual(instance.method, "get-scene")
        self.assertIsNone(instance.params)

    def test_get_bounds(self) -> None:
        instance = MockInstance(self.mock_scene_message())
        test = brayns.get_bounds(instance)
        self.assertEqual(test, self.mock_scene().bounds)
        self.assertEqual(instance.method, "get-scene")
        self.assertIsNone(instance.params)

    def test_get_models(self) -> None:
        instance = MockInstance(self.mock_scene_message())
        test = brayns.get_models(instance)
        self.assertEqual(test, self.mock_scene().models)
        self.assertEqual(instance.method, "get-scene")
        self.assertIsNone(instance.params)

    def test_instantiate_model(self) -> None:
        instance = MockInstance([mock_model_message()])
        models = brayns.instantiate_model(
            instance,
            model_id=0,
            transforms=[mock_transform()],
        )
        self.assertEqual(len(models), 1)
        self.assertEqual(models[0], mock_model())
        self.assertEqual(instance.method, "instantiate-model")
        self.assertEqual(
            instance.params,
            {
                "model_id": 0,
                "transforms": [mock_transform_message()],
            },
        )

    def test_update_model(self) -> None:
        instance = MockInstance(mock_model_message())
        model = brayns.update_model(
            instance,
            model_id=0,
            transform=mock_transform(),
            visible=True,
        )
        self.assertEqual(model, mock_model())
        self.assertEqual(instance.method, "update-model")
        self.assertEqual(
            instance.params,
            {
                "model_id": 0,
                "model": {
                    "is_visible": True,
                    "transform": mock_transform_message(),
                },
            },
        )

    def mock_scene(self) -> brayns.Scene:
        return brayns.Scene(
            bounds=mock_bounds(),
            models=[
                mock_model(),
                dataclasses.replace(mock_model(), id=1),
            ],
        )

    def mock_scene_message(self) -> dict[str, Any]:
        return {
            "bounds": mock_bounds_message(),
            "models": [
                mock_model_message(),
                mock_model_message() | {"model_id": 1},
            ],
        }
