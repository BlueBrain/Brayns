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

from typing import Any

import brayns


def mock_bounds() -> brayns.Bounds:
    return brayns.Bounds(
        min=brayns.Vector3(0, 1, 2),
        max=brayns.Vector3(3, 4, 5),
    )


def mock_bounds_message() -> dict[str, Any]:
    return {
        "min": [0, 1, 2],
        "max": [3, 4, 5],
    }


def mock_transform() -> brayns.Transform:
    return brayns.Transform(
        translation=brayns.Vector3(0, 1, 2),
        rotation=brayns.Rotation.identity,
        scale=brayns.Vector3(7, 8, 9),
    )


def mock_transform_message() -> dict[str, Any]:
    return {
        "translation": [0, 1, 2],
        "rotation": [0, 0, 0, 1],
        "scale": [7, 8, 9],
    }


def mock_model() -> brayns.Model:
    return brayns.Model(
        id=0,
        type="test",
        bounds=mock_bounds(),
        info={"test": "1"},
        visible=True,
        transform=mock_transform(),
    )


def mock_model_message() -> dict[str, Any]:
    return {
        "model_id": 0,
        "model_type": "test",
        "bounds": mock_bounds_message(),
        "info": {"test": "1"},
        "is_visible": True,
        "transform": mock_transform_message(),
    }


def mock_view() -> brayns.View:
    return brayns.View(
        position=brayns.Vector3(0, 1, 2),
        target=brayns.Vector3(3, 4, 5),
        up=brayns.Vector3(6, 7, 8),
    )


def mock_view_message() -> dict[str, Any]:
    return {
        "position": [0, 1, 2],
        "target": [3, 4, 5],
        "up": [6, 7, 8],
    }


def mock_schema() -> brayns.JsonSchema:
    return brayns.JsonSchema(
        title="test1",
        description="test2",
        type=brayns.JsonType.NULL,
        read_only=True,
        write_only=True,
        default=123,
        minimum=0,
        maximum=10,
        items=brayns.JsonSchema(
            type=brayns.JsonType.UNDEFINED,
            additional_properties=False,
        ),
        min_items=1,
        max_items=3,
        properties={
            "1": brayns.JsonSchema(type=brayns.JsonType.BOOLEAN),
            "2": brayns.JsonSchema(type=brayns.JsonType.INTEGER),
        },
        required=["1", "2"],
        additional_properties=False,
        one_of=[
            brayns.JsonSchema(type=brayns.JsonType.NUMBER),
            brayns.JsonSchema(type=brayns.JsonType.STRING),
            brayns.JsonSchema(type=brayns.JsonType.OBJECT),
            brayns.JsonSchema(type=brayns.JsonType.ARRAY),
        ],
        enum=["test", 123],
    )


def mock_schema_message() -> dict[str, Any]:
    return {
        "title": "test1",
        "description": "test2",
        "type": "null",
        "readOnly": True,
        "writeOnly": True,
        "default": 123,
        "minimum": 0,
        "maximum": 10,
        "items": {
            "additionalProperties": False,
        },
        "minItems": 1,
        "maxItems": 3,
        "properties": {
            "1": {"type": "boolean"},
            "2": {"type": "integer"},
        },
        "required": ["1", "2"],
        "additionalProperties": False,
        "oneOf": [
            {"type": "number"},
            {"type": "string"},
            {"type": "object"},
            {"type": "array"},
        ],
        "enum": ["test", 123],
    }
