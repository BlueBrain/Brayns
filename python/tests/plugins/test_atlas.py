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

import unittest
from typing import Any

import brayns
from tests.mock_instance import MockInstance
from tests.mock_messages import mock_model, mock_model_message


class TestAtlas(unittest.TestCase):
    def test_get_atlas_usecases(self) -> None:
        use_cases = [("test1", brayns.JsonSchema()), ("test2", brayns.JsonSchema())]
        instance_use_cases: list[dict[str, Any]] = [
            {
                "name": "test1",
                "params_schema": {},
            },
            {
                "name": "test2",
                "params_schema": {},
            },
        ]
        instance = MockInstance(instance_use_cases)
        test = brayns.get_atlas_usecases(instance, 2)
        self.assertEqual(test, use_cases)
        self.assertEqual(instance.method, "get-available-atlas-usecases")
        self.assertEqual(instance.params, {"model_id": 2})

    def test_run(self) -> None:
        instance = MockInstance(mock_model_message())
        usecase = brayns.AtlasColumnHighlight(
            position=brayns.ColumnPosition(1, 2),
            color=brayns.Color4.red,
        )
        test = usecase.run(instance, 2)
        self.assertEqual(test, mock_model())
        self.assertEqual(instance.method, "visualize-atlas-usecase")
        self.assertEqual(
            instance.params,
            {
                "model_id": 2,
                "use_case": "Highlight columns",
                "params": {
                    "xz_coordinate": [1, 2],
                    "color": [1, 0, 0, 1],
                    "neighbours": [],
                },
            },
        )

    def test_density(self) -> None:
        self.assertEqual(brayns.AtlasDensity.name, "Density")

    def test_flatmap_areas(self) -> None:
        self.assertEqual(brayns.AtlasFlatmapAreas.name, "Flatmap areas")

    def test_column_highlight(self) -> None:
        self.assertEqual(brayns.AtlasColumnHighlight.name, "Highlight columns")
        loader = brayns.AtlasColumnHighlight(
            brayns.ColumnPosition(1, 2),
            color=brayns.Color4.red,
            neighbors=[
                brayns.ColumnNeighbor(
                    position=brayns.ColumnPosition(3, 4),
                    color=brayns.Color4.green,
                ),
                brayns.ColumnNeighbor(
                    position=brayns.ColumnPosition(5, 6),
                    color=brayns.Color4.blue,
                ),
            ],
        )
        self.assertEqual(
            loader.get_properties(),
            {
                "xz_coordinate": [1, 2],
                "color": [1, 0, 0, 1],
                "neighbours": [
                    {"relative_xz": [3, 4], "color": [0, 1, 0, 1]},
                    {"relative_xz": [5, 6], "color": [0, 0, 1, 1]},
                ],
            },
        )

    def test_layer_distance(self) -> None:
        self.assertEqual(brayns.AtlasLayerDistance.name, "Layer distance")
        usecase = brayns.AtlasLayerDistance(brayns.AtlasDistanceType.UPPER)
        self.assertEqual(usecase.get_properties(), {"type": "upper"})

    def test_orientation_field(self) -> None:
        self.assertEqual(brayns.AtlasOrientationField.name, "Orientation field")

    def test_shell_outline(self) -> None:
        self.assertEqual(brayns.AtlasShellOutline.name, "Outline mesh shell")
